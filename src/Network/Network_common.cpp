#include "Network_common.h"
#include <signal.h>
#include <iostream>
#include <assert.h>

namespace pwskoag
{ 
	const size_t Packet::MAXSIZE=4096;

	Socket::Socket(IpAddress& ip, ushort port, Type type) : m_Id(0), ip(ip), port(port), fd(0), type(type)
	{
		fd=socket(AF_INET, type, type==TCP ? IPPROTO_TCP : IPPROTO_UDP);
		if(fd<=0) throw std::runtime_error("Failed to create socket.");
		memset(&addr, 0, sizeof(addr));
		addr.sin_family=AF_INET;
		addr.sin_port=htons(port);
		addr.sin_addr=ip.addr;
		int yes=1;
		setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
	}

	Socket::Socket(ushort port, Type type) : m_Id(0), ip(), port(port), fd(0), type(type)
	{
		fd=socket(AF_INET, type, type==TCP ? IPPROTO_TCP : IPPROTO_UDP);
		if(fd<=0) throw std::runtime_error(Error("Socket"));
		memset(&addr, 0, sizeof(addr));
		addr.sin_family=AF_INET;
		addr.sin_port=htons(port);
		addr.sin_addr.s_addr=htonl(INADDR_ANY);
		int yes=1;
		setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
	}

	void Socket::Bind()
	{
		socklen_t len=sizeof(addr);
		if(bind(fd, (struct sockaddr*)&addr, len)!=0) throw std::runtime_error(Error("Bind", type));
	}

	bool TcpSocket::Receive(Packet& p)
	{
		uchar buf[Packet::MAXSIZE];
		int r=recv(fd, buf, Packet::MAXSIZE, 0);
		if(r<=0) return false;
		for(int i=0;i<r;++i)p<<buf[i];
		return true;
	}
	bool UdpSocket::Receive(Packet& p, IpAddress* ip, ushort* port)
	{
		uchar buf[Packet::MAXSIZE];
		struct sockaddr_in a;
		socklen_t len=sizeof(a);
		int r=recvfrom(fd, buf, Packet::MAXSIZE, 0, (struct sockaddr*)&a, &len);
		if(r<0) return false;
		for(int i=0;i<r;++i)p<<buf[i];
		if(ip) *ip=IpAddress(a.sin_addr);
		if(port) *port=ntohs(a.sin_port);
		return true;
	}

	void TcpSocket::Connect()
 	{
		fd_set set;
		FD_ZERO(&set);
		FD_SET(fd, &set);
		socklen_t len=sizeof(addr);
		int ret=-1;
		ret=connect(fd, (sockaddr*)&addr, len);
		if(ret<0)Error("Connect:",type);
	}

	bool TcpSocket::Send(Packet& p)
	{
		fd_set set;
		FD_ZERO(&set);
		FD_SET(fd, &set);
		struct timeval tv;
		tv.tv_sec=2;
		tv.tv_usec=0;
		int bytes=-1;
		int ret=select(fd+1, NULL, &set, NULL, &tv);
		if(ret>0 && FD_ISSET(fd, &set))
		{
			FD_CLR(fd, &set);
			bytes=send(fd, p.RawData(), p.Size(), 0);
			if(bytes==-1)
			{
				if(errno==EPIPE || errno==ECONNRESET || errno==ENOTCONN)
				{
					std::cerr << "Connection lost." << std::endl;
					return false;
				}
				perror("SEND");
			}
		}
		else {std::cout << "Something went wrong." << std::endl; close(fd); exit(1);}
		assert(bytes==p.Size());
		p.Clear();
		return true;
	}

	bool UdpSocket::Send(Packet& p, IpAddress& ip, ushort port)
	{
		struct sockaddr_in a;
		a.sin_family=AF_INET;
		a.sin_port=htons(port);
		a.sin_addr=ip.addr;
		socklen_t len=sizeof(a);
		int r=sendto(fd, p.RawData(), p.Size(), 0, (struct sockaddr*)&a, len);
		p.Clear();
		return true;
	}

	TcpSocket* TcpSocket::Accept()
	{
		socklen_t len=sizeof(addr);
		int newfd=accept(fd, (sockaddr*)&addr, &len);
		if(newfd<0) return NULL;
		IpAddress ip(addr.sin_addr);
		return new TcpSocket(ip, htons(addr.sin_port), Socket::TCP, newfd);
	}


	void Selector::Remove(Socket& s)
	{
		for(std::vector<int>::iterator it=fd_ints.begin(); it!=fd_ints.end(); ++it)
		{
			if(*it==s.fd)
			{
				std::cout << "Erasing: " << *it << std::endl;
				it=fd_ints.erase(it);
				break;
			}
		}
		std::sort(fd_ints.begin(), fd_ints.end());
	}

	int Selector::Wait(uint timeoutms)
	{
		if(fd_ints.size())
		{
			struct timeval tv;
			tv.tv_sec=timeoutms/1000;
			tv.tv_usec=(timeoutms%1000)*1000;
			FD_ZERO(&fds);
			for(std::vector<int>::iterator it=fd_ints.begin(); it!=fd_ints.end(); ++it) FD_SET(*it, &fds);
			return select(fd_ints.back()+1, &fds, NULL, NULL, &tv);
		}
		else
		{
			msSleep(timeoutms);
			return 0;
		}
	};

	int Selector::WaitWrite(uint timeoutms)
	{
		if(fd_ints.size())
		{
			struct timeval tv;
			tv.tv_sec=timeoutms/1000;
			tv.tv_usec=(timeoutms%1000)*1000;
			FD_ZERO(&fds);
			for(std::vector<int>::iterator it=fd_ints.begin(); it!=fd_ints.end(); ++it) FD_SET(*it, &fds);
			return select(fd_ints.back()+1, NULL, &fds, NULL, &tv);
		}
		else
		{
			msSleep(timeoutms);
			return 0;
		}
	};

	int Selector::WaitReadWrite(uint timeoutms)
	{
		if(fd_ints.size())
		{
			struct timeval tv;
			tv.tv_sec=timeoutms/1000;
			tv.tv_usec=(timeoutms%1000)*1000;
			FD_ZERO(&fds);
			for(std::vector<int>::iterator it=fd_ints.begin(); it!=fd_ints.end(); ++it) FD_SET(*it, &fds);
			return select(fd_ints.back()+1, &fds, &fds, NULL, &tv);
		}
		else
		{
			msSleep(timeoutms);
			return 0;
		}
	};

	void IpAddress::StrToAddr(const char* a)
	{
		if(std::string(a)=="localhost") a="127.0.0.1";
		if(inet_aton(a, &addr)==0)
		{
			std::string errmsg=("IpAddress is not a valid address.");
			throw std::runtime_error(errmsg);
		}
	}

	void Server::Start()
	{
		Lock lock(selfMutex);
		if(!selfThread)
		{
			stopNow=false;
			selfThread = new Thread(Server::ServerInitializer, this);
		}
		else std::cerr << "Server already running!" << std::endl;
	}
	void Server::Stop()
	{
		Lock lock(selfMutex);
		stopNow=true;
		if(selfThread) {selfThread->Join(); delete selfThread; selfThread=NULL;}
		else std::cerr << "Server already stopped!" << std::endl;
	}
	void Server::ForceStop()
	{
		//Lock lock(selfMutex);
		//if(selfThread) {selfThread->Terminate(); delete selfThread; selfThread=NULL;}
	}
	void Server::ServerInitializer(void* args)
	{
		Server* s = (Server*)args;
		s->ServerLoop();
	}
	Server::~Server()
	{
		if(selfThread) std::cerr << "!! Server not stopped and still shutting down !!" << std::endl;
	}
	
	void Client::Start()
	{
		Lock lock(selfMutex);
		if(!selfThread)
		{
			selfThread = new Thread(Client::ClientInitializer, this);
		}
		else std::cerr << "Client already running!" << std::endl;
	}
	void Client::Stop()
	{
		Lock lock(selfMutex);
		stopNow=true;
		if(selfThread) {std::cout << "Disconnecting...";std::cout.flush(); delete selfThread; selfThread=NULL;std::cout<<"DONE!"<<std::endl;}
		else std::cerr << "Client already stopped!" << std::endl;
	}
	void Client::ForceStop()
	{
		//Lock lock(selfMutex);
		//if(selfThread) {selfThread->Terminate(); delete selfThread; selfThread=NULL;}
	}
	void Client::ClientInitializer(void* args)
	{
		Client* s = (Client*)args;
		s->ClientLoop();
	}
	Client::~Client()
	{
		if(selfThread) std::cerr << "!! Client not stopped and still shutting down !!" << std::endl;
	}
}
