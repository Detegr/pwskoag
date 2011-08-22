#pragma once
#include <SFML/Network.hpp>
#include <unordered_map>
#include <stdexcept>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>
#include "Base.h"
#include "Network_commands.h"

namespace Network
{
	class IpAddress
	{
		friend class Socket;
		private:
			struct in_addr addr;
			void StrToAddr(const char* a);
		public:
			IpAddress() : addr() {}
			IpAddress(const char* a) {StrToAddr(a);}
			IpAddress(const IpAddress& rhs) {addr=rhs.addr;}
			const IpAddress& operator=(const char* a) {StrToAddr(a);}
			bool operator==(const IpAddress& rhs) const {return strncmp(toString().c_str(), rhs.toString().c_str(), 15)==0;}
			bool operator==(const char* rhs) const {return strncmp(toString().c_str(), rhs, 15)==0;}
			std::string toString() const {return std::string(inet_ntoa(addr));}
			friend std::ostream& operator<<(std::ostream& o, const IpAddress& rhs) {o << rhs.toString(); return o;}
	};

	class Socket
	{
		protected:
			IpAddress	ip;
			ushort		port;
			int			fd;
			int			type;
			sockaddr_in	addr;
		public:
			enum Type
			{
				TCP=SOCK_STREAM,
				UDP=SOCK_DGRAM
			};
			Socket(IpAddress& ip, ushort port, Type type) : ip(ip), port(port), fd(0), type(type)
			{
				if(fd=socket(AF_INET, type, type==Type::TCP ? IPPROTO_TCP : IPPROTO_UDP)<0) throw std::runtime_error("Failed to create socket.");
				addr.sin_family=AF_INET;
				addr.sin_port=htons(port);
				addr.sin_addr=ip.addr;
			}
			Socket(ushort port, Type type) : ip(), port(port), fd(0), type(type)
			{
				fd=socket(AF_INET, type, type==Type::TCP ? IPPROTO_TCP : IPPROTO_UDP);
				if(fd<0) throw std::runtime_error(Error("Socket"));
				addr.sin_family=AF_INET;
				addr.sin_port=htons(port);
				addr.sin_addr.s_addr=INADDR_ANY;
			}
			void SetBlocking(bool b) {int flags; if(flags=(fcntl(fd, F_GETFL, 0))==-1) flags=0; fcntl(fd, F_SETFL, b?flags&O_NONBLOCK:flags|O_NONBLOCK);}
	};

	class TcpSocket : public Socket
	{
		public:
			TcpSocket(IpAddress& ip, ushort port, Socket::Type type) : Socket(ip, port, type) {}
			TcpSocket(ushort port, Socket::Type type) : Socket(port, type) {}
			void Bind() {socklen_t len=sizeof(addr); if(bind(fd, (sockaddr*)&addr, len)<0) throw std::runtime_error(Error("Bind"));}
			void Listen(int buffer=10) {if(listen(fd,buffer)<0) throw std::runtime_error(Error("Listen"));}
			void Accept() {socklen_t len=sizeof(addr); if(accept(fd, (sockaddr*)&addr, &len)<0) throw std::runtime_error(Error("Accept"));}
			void Read(Packet& p) {}
	};

	class Packet
	{
		private:
			std::vector<uchar> data;
			void Append(const void* d, size_t len) {data.resize(data.size()+len); memcpy(&data[data.size()-len], d, len);}
			void Pop(size_t bytes) {data.erase(data.begin(), data.begin()+bytes);}
		public:
			void* RawData() const {return (void*)&data[0];}
			void Clear() {data.clear();}
			void operator<<(const char* str) {Append(str, strlen(str)+1);}
			void operator<<(const std::string& str){Append(str.c_str(), str.length()+1);}
			void operator>>(char* str) {strcpy(str, (char*)&data[0]); Pop(strlen(str)+1);}
			void operator>>(std::string& str) {str=(char*)&data[0]; Pop(str.length()+1);}
			template <class type> void operator<<(type x) {Append(&x, sizeof(type));}
			template <class type> void operator>>(type& x) {x=*(type*)&data[0]; Pop(sizeof(type));}
	};

	// Functions for sending and appending.
	static void Append(Command c, sf::Packet& p) {p<<(uchar)c;}
	template <class type> void Append(type& t, sf::Packet& p){p<<t;}
	template <class type> void Append(Command c, type& t, sf::Packet& p){Append(c,p);Append(t,p);}

	// Tcp-functions
	static void TcpSend(Command c, sf::TcpSocket* sock, sf::Packet& p)
	{
		p << (uchar)c << (uchar)Command::EOP;
		sock->Send(p);
		p.Clear();
	}
	static void TcpSend(sf::TcpSocket* sock, sf::Packet& p) {sock->Send(p); p.Clear();}
	template <class type> void TcpSend(Command c, type t, sf::TcpSocket* sock, sf::Packet& p)
	{
			p.Clear();
			Append(c,t,p); Append(Command::EOP, p); TcpSend(sock,p);
	}

	// Udp-functions
	static void UdpSend(Command c, sf::UdpSocket* sock, sf::IpAddress& ip, ushort port, sf::Packet& p)
	{
		p << (uchar)c << (uchar)Command::EOP;
		sock->Send(p, ip, port);
	}
	static void UdpSend(sf::UdpSocket* sock, sf::IpAddress& ip, ushort port, sf::Packet& p) {sock->Send(p,ip,port); p.Clear();}
	template <class type>
	void UdpSend(Command c, type t, sf::UdpSocket* sock, sf::IpAddress& ip, ushort port, sf::Packet& p)
	{
		p.Clear();
		Append(c, t, p); Append(Command::EOP, p); UdpSend(sock, ip, port, p);
	}

	class AutoSender
	{
		protected:
			bool											stopNow;
			sf::Thread*										selfThread;
			sf::Mutex										autoSendMutex;
			sf::Mutex										selfMutex;
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
				sf::Lock lock(autoSendMutex);
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
			sf::Mutex 		selfMutex;
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
			sf::Mutex		selfMutex;
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
