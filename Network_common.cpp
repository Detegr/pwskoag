#include "Network_common.h"
#include <signal.h>
#include <iostream>

namespace Network
{ 
	const size_t Packet::MAXSIZE=4096;

	Socket::Socket(IpAddress& ip, ushort port, Type type) : ip(ip), port(port), fd(0), type(type)
	{
		fd=socket(AF_INET, type, type==Type::TCP ? IPPROTO_TCP : IPPROTO_UDP);
		std::cout << (type==Type::TCP ? IPPROTO_TCP : IPPROTO_UDP) << std::endl;
		if(fd<=0) throw std::runtime_error("Failed to create socket.");
		memset(&addr, 0, sizeof(addr));
		addr.sin_family=AF_INET;
		addr.sin_port=htons(port);
		addr.sin_addr=ip.addr;
		int yes=1;
		setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
	}

	Socket::Socket(ushort port, Type type) : ip(), port(port), fd(0), type(type)
	{
		//fd=socket(AF_INET, type, type==Type::TCP ? IPPROTO_TCP : IPPROTO_UDP);
		fd=socket(AF_INET, SOCK_STREAM, 0);
		if(fd<=0) throw std::runtime_error(Error("Socket"));
		memset(&addr, 0, sizeof(addr));
		addr.sin_family=AF_INET;
		addr.sin_port=htons(port);
		addr.sin_addr.s_addr=htonl(INADDR_ANY);

		int yes=1;
		setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
	}

	bool TcpSocket::Receive(Packet& p)
	{
		uchar buf[Packet::MAXSIZE];
		int r=recv(fd, buf, Packet::MAXSIZE, 0);
		if(r<0) return false;
		p<<buf;
		return true;
	}
	bool UdpSocket::Receive(Packet& p, IpAddress& ip, ushort port)
	{
		uchar buf[Packet::MAXSIZE];
		struct sockaddr_in a;
		a.sin_family=AF_INET;
		a.sin_port=port;
		a.sin_addr=ip.addr;
		socklen_t len=sizeof(a);
		int r=recvfrom(fd, buf, Packet::MAXSIZE, NULL, (struct sockaddr*)&a, &len);
		if(r<0) return false;
		p<<buf;
		return true;
	}

	void TcpSocket::Connect()
 	{
		fd_set set;
		FD_ZERO(&set);
		FD_SET(fd, &set);
		socklen_t len=sizeof(addr);
		while(select(fd+1, NULL, &set, NULL, NULL)<=0) {}
		if(connect(fd, (sockaddr*)&addr, len)!=0 && errno!=EINPROGRESS) throw std::runtime_error(Error("Connect:",type));
		while(select(fd+1, NULL, &set, NULL, NULL)<=0) {}
		std::cout << "Connected!" << std::endl;
	}

	void TcpSocket::Send(Packet& p)
	{
		fd_set set;
		FD_ZERO(&set);
		FD_SET(fd, &set);
		while(true)
		{
			int ret=select(fd+1, NULL, &set, NULL, NULL);
			if(ret>0)
			{
				if(send(fd, p.RawData(), p.Size(), 0)<=0) throw std::runtime_error(Error("Send", type));
				break;
			}
		}
		p.Clear();
	}

	void UdpSocket::Send(Packet& p, IpAddress& ip, ushort port)
	{
		struct sockaddr_in a;
		a.sin_family=AF_INET;
		a.sin_port=port;
		a.sin_addr=ip.addr;
		socklen_t len=sizeof(addr);
		sendto(fd, p.RawData(), p.Size(), NULL, (struct sockaddr*)&a, len);
		p.Clear();
	}

	TcpSocket* TcpSocket::Accept()
	{
		socklen_t len=sizeof(addr);
		int newfd=accept(fd, (sockaddr*)&addr, &len);
		if(newfd<0) return NULL;
		return new TcpSocket(ip, port, Socket::Type::TCP, newfd);
	}

	void Selector::Wait(uint timeoutms)
	{
		struct timeval tv;
		tv.tv_sec=timeoutms/1000;
		tv.tv_usec=(timeoutms%1000)*1000;
		int ret=select(highest+1, &fds, NULL, NULL, &tv);
		std::cout << "Select returned: " << ret << std::endl;
	};

	void IpAddress::StrToAddr(const char* a)
	{
		if(std::string(a)=="localhost") a="127.0.0.1";
		if(inet_aton(a, &addr)==0)
		{
			std::string errmsg=("IpAddress  is not a valid address.");
			throw std::runtime_error(errmsg);
		}
	}

	void AutoSender::AutoSendInitializer(void* args)
	{
		AutoSender* s=(AutoSender*) args;
		s->AutoSendLoop();
	}
	void AutoSender::Start()
	{
		Concurrency::Lock lock(selfMutex);
		if(!selfThread)
		{
			stopNow=false;
			selfThread = new sf::Thread(AutoSendInitializer, this);
			selfThread->Launch();
		}
		else std::cerr << "AutoSender already running!" << std::endl;
	}
	void AutoSender::Stop()
	{
		Concurrency::Lock lock(selfMutex);
		stopNow=true;
		if(selfThread) {delete selfThread; selfThread=NULL;}
		else std::cerr << "AutoSender already stopped!" << std::endl;
	}
	void AutoSender::ForceStop()
	{
		Concurrency::Lock lock(selfMutex);
		if(selfThread) {selfThread->Terminate(); delete selfThread; selfThread=NULL;}
	}
	void Server::Start()
	{
		Concurrency::Lock lock(selfMutex);
		if(!selfThread)
		{
			stopNow=false;
			selfThread = new sf::Thread(Server::ServerInitializer, this);
			selfThread->Launch();
		}
		else std::cerr << "Server already running!" << std::endl;
	}
	void Server::Stop()
	{
		Concurrency::Lock lock(selfMutex);
		stopNow=true;
		if(selfThread) {delete selfThread; selfThread=NULL;}
		else std::cerr << "Server already stopped!" << std::endl;
	}
	void Server::ForceStop()
	{
		Concurrency::Lock lock(selfMutex);
		if(selfThread) {selfThread->Terminate(); delete selfThread; selfThread=NULL;}
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
		Concurrency::Lock lock(selfMutex);
		if(!selfThread)
		{
			selfThread = new sf::Thread(Client::ClientInitializer, this);
			selfThread->Launch();
		}
		else std::cerr << "Client already running!" << std::endl;
	}
	void Client::Stop()
	{
		Concurrency::Lock lock(selfMutex);
		stopNow=true;
		if(selfThread) {selfThread->Wait(); delete selfThread; selfThread=NULL;}
		else std::cerr << "Client already stopped!" << std::endl;
	}
	void Client::ForceStop()
	{
		Concurrency::Lock lock(selfMutex);
		if(selfThread) {selfThread->Terminate(); delete selfThread; selfThread=NULL;}
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
