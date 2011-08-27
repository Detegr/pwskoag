#pragma once
#include <SFML/Network.hpp>
#include <unordered_map>
#include <stdexcept>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>
#include "Base.h"
#include "Network_commands.h"
#include "Concurrency.h"

namespace Network
{
	class IpAddress
	{
		friend class Socket;
		friend class TcpSocket;
		friend class UdpSocket;
		private:
			struct in_addr addr;
			void StrToAddr(const char* a);
		public:
			IpAddress() : addr() {}
			IpAddress(const char* a) {StrToAddr(a);}
			IpAddress(const IpAddress& rhs) {addr=rhs.addr;}
			const IpAddress&		operator=(const char* a) {StrToAddr(a); return *this;}
			bool					operator==(const IpAddress& rhs) const {return strncmp(toString().c_str(), rhs.toString().c_str(), 15)==0;}
			bool					operator==(const char* rhs) const {return strncmp(toString().c_str(), rhs, 15)==0;}
			std::string 			toString() const {return std::string(inet_ntoa(addr));}
			friend std::ostream& 	operator<<(std::ostream& o, const IpAddress& rhs) {o << rhs.toString(); return o;}
	};

	class Packet
	{
		friend class Socket;
		private:
			std::vector<uchar>	data;
			void 				Append(const void* d, size_t len) {data.resize(data.size()+len); memcpy(&data[data.size()-len], d, len);}
			void				Pop(size_t bytes) {data.erase(data.begin(), data.begin()+bytes);}
		public:
			static const size_t				MAXSIZE;
			const uchar* 					RawData() const {return &data[0];}
			void 							Clear() {data.clear();}
			size_t							Size() const {return data.size();}
			Packet&							operator<<(const char* str) {Append(str, strlen(str)+1);}
			Packet&							operator<<(const std::string& str){Append(str.c_str(), str.length()+1); return *this;}
			Packet&							operator>>(char* str) {strcpy(str, (char*)&data[0]); Pop(strlen(str)+1); return *this;}
			Packet&							operator>>(std::string& str) {str=(char*)&data[0]; Pop(str.length()+1); return *this;}
			template <class type> Packet&	operator<<(type x) {Append(&x, sizeof(type)); return *this;}
			template <class type> Packet&	operator>>(type& x) {x=*(type*)&data[0]; Pop(sizeof(type)); return *this;}
	};

	class Socket
	{
		friend class Selector;
		protected:
			IpAddress	ip;
			ushort		port;
			int			type;
			sockaddr_in	addr;

			Socket() {}
		public:
			int			fd;
			enum Type
			{
				TCP=SOCK_STREAM,
				UDP=SOCK_DGRAM
			};
			Socket(IpAddress& ip, ushort port, Type type);
			Socket(ushort port, Type type);
			Socket(const Socket& s) {*this=s;}
			const Socket& operator=(const Socket& s) {ip=s.ip;port=s.port;fd=s.fd;type=s.type;addr=s.addr; return *this;}
			void SetBlocking(bool b) {int flags; if(flags=(fcntl(fd, F_GETFL, 0))==-1) flags=0; fcntl(fd, F_SETFL, b?flags&O_NONBLOCK:flags|O_NONBLOCK);}
			void Bind() {socklen_t len=sizeof(addr); if(bind(fd, (struct sockaddr*)&addr, len)!=0) throw std::runtime_error(Error("Bind", type));}
			void Close() {close(fd);}
			void ForceClose() {shutdown(fd, SHUT_RDWR);Close();}
			const IpAddress&	GetIp() const {return ip;}
			const ushort		GetPort() const {return port;}
	};

	class TcpSocket : public Socket
	{
		private:
			TcpSocket(IpAddress& ip, ushort port, Type type, int fd) : Socket(ip, port, type) {this->fd=fd;}
		public:
			TcpSocket() {}
			TcpSocket(IpAddress& ip, ushort port) : Socket(ip, port, Type::TCP) {}
			TcpSocket(IpAddress&& ip, ushort port) : Socket(ip, port, Type::TCP) {}
			TcpSocket(ushort port) : Socket(port, Type::TCP) {}
			void Listen(int buffer=5) {if(listen(fd,buffer)!=0) throw std::runtime_error(Error("Listen"));}
			void Connect();
			void Disconnect() {Close();}
			TcpSocket* Accept(); 
			void Send(Packet& p); 
			bool Receive(Packet& p); 
	};

	struct UdpSocket : public Socket
	{
		UdpSocket() {}
		UdpSocket(IpAddress& ip, ushort port) : Socket(ip, port, Type::UDP) {}
		void Send(Packet& p, IpAddress& ip, ushort port);
		bool Receive(Packet& p, IpAddress& ip, ushort port); 
	};

	class Selector
	{
		private:
			int highest;
			fd_set fds;
		public:
			Selector() : highest(0) {FD_ZERO(&fds);}
			void Add(Socket& s) {if(s.fd>highest){highest=s.fd; std::cout << "Highest: " << s.fd << std::endl;} FD_SET(s.fd, &fds);}
			bool IsReady(Socket& s) {return FD_ISSET(s.fd, &fds)!=0;}
			void Wait(uint timeoutms);
			void Remove(Socket& s) {FD_CLR(s.fd, &fds);}
	};

	// Functions for sending and appending.
	static void Append(Command c, Packet& p) {p<<(uchar)c;}
	template <class type> void Append(type& t, Packet& p){p<<t;}
	template <class type> void Append(Command c, type& t, Packet& p){Append(c,p);Append(t,p);}

	// Tcp-functions
	static void TcpSend(Command c, TcpSocket* sock, Packet& p)
	{
		p << (uchar)c << (uchar)Command::EOP;
		sock->Send(p);
		p.Clear();
	}
	static void TcpSend(TcpSocket* sock, Packet& p) {sock->Send(p); p.Clear();}
	template <class type> void TcpSend(Command c, type t, TcpSocket* sock, Packet& p)
	{
			p.Clear();
			Append(c,t,p); Append(Command::EOP, p); TcpSend(sock,p);
	}

	// Udp-functions
	static void UdpSend(Command c, UdpSocket* sock, IpAddress& ip, ushort port, Packet& p)
	{
		p << (uchar)c << (uchar)Command::EOP;
		sock->Send(p, ip, port);
	}
	static void UdpSend(UdpSocket* sock, IpAddress& ip, ushort port, Packet& p) {sock->Send(p,ip,port); p.Clear();}
	template <class type>
	void UdpSend(Command c, type t, UdpSocket* sock, IpAddress& ip, ushort port, Packet& p)
	{
		p.Clear();
		Append(c, t, p); Append(Command::EOP, p); UdpSend(sock, ip, port, p);
	}

	class AutoSender
	{
		protected:
			bool											stopNow;
			sf::Thread*										selfThread;
			Concurrency::Mutex											autoSendMutex;
			Concurrency::Mutex											selfMutex;
			static void										AutoSendInitializer(void* args);
			std::unordered_map<uchar, std::vector <void*> >	objectsToSend;
			virtual void									AutoSendLoop()=0;
			void											Start();
			void											Stop();
			void											ForceStop();
			AutoSender() : stopNow(false), selfThread(NULL) {}
		public:
			template <class type> void AutoSend(Command c, type* t)
			{
				Concurrency::Lock lock(autoSendMutex);
				objectsToSend[(uchar)c].push_back(t);
			}
	};
	/*
	 * Server class
	 * Meant to be inherited. Includes routines for starting and
	 * stopping a server.
	 *
	 * Instantable server class must override ServerLoop(void*).
	 */
	class Server
	{
		protected:
			bool 			stopNow;
			uint 			serverPort;
			Concurrency::Mutex	 		selfMutex;
			sf::Thread* 	selfThread;
			static void 	ServerInitializer(void* args);
			virtual 		~Server();
			virtual void	ServerLoop()=0;

			Server(ushort port) : stopNow(false), serverPort(port), selfThread(NULL) {};
		public:
			virtual void 	Start();
			virtual void 	Stop();
			virtual void 	ForceStop();
			bool IsRunning() const { return selfThread!=NULL; }
	};
	
	/*
	 * Client class
	 * Meant to be inherited. Includes routines for starting and
	 * stopping a client.
	 *
	 * Instantable client class must override ServerLoop(void*).
	 */
	class Client
	{
		protected:
			bool			stopNow;
			uint 			serverPort;
			Concurrency::Mutex			selfMutex;
			sf::Thread*		selfThread;
			static void		ClientInitializer(void* args);
			virtual 		~Client();
			virtual void 	ClientLoop()=0;

			Client() : stopNow(false), serverPort(), selfThread(NULL) {};
		public:
			virtual void 	Start();
			virtual void 	Stop();
			virtual void 	ForceStop();
			virtual void 	Connect(const char* addr, ushort port)=0;
			virtual void 	Disconnect()=0;
			bool 	     	IsRunning() const { return selfThread!=NULL; }
	};

}
