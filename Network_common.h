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
			const					IpAddress& operator=(const char* a) {StrToAddr(a);}
			bool					operator==(const IpAddress& rhs) const {return strncmp(toString().c_str(), rhs.toString().c_str(), 15)==0;}
			bool					operator==(const char* rhs) const {return strncmp(toString().c_str(), rhs, 15)==0;}
			std::string 			toString() const {return std::string(inet_ntoa(addr));}
			friend std::ostream& 	operator<<(std::ostream& o, const IpAddress& rhs) {o << rhs.toString(); return o;}
	};

	class Packet
	{
		friend class Socket;
		private:
			static size_t		MAXSIZE;
			std::vector<uchar>	data;
			void 				Append(const void* d, size_t len) {data.resize(data.size()+len); memcpy(&data[data.size()-len], d, len);}
			void				Pop(size_t bytes) {data.erase(data.begin(), data.begin()+bytes);}
		public:
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
	size_t Packet::MAXSIZE=4096;

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
			Socket(IpAddress& ip, ushort port, Type type);
			Socket(ushort port, Type type);
			void SetBlocking(bool b) {int flags; if(flags=(fcntl(fd, F_GETFL, 0))==-1) flags=0; fcntl(fd, F_SETFL, b?flags&O_NONBLOCK:flags|O_NONBLOCK);}
			void Bind() {socklen_t len=sizeof(addr); if(bind(fd, (sockaddr*)&addr, len)<0) throw std::runtime_error(Error("Bind"));}
			void Receive(Packet& p); 
			void Send(Packet& p); 
	};

	class TcpSocket : public Socket
	{
		private:
			TcpSocket(IpAddress& ip, ushort port, Type type, int fd) : Socket(ip, port, type) {this->fd=fd;}
		public:
			TcpSocket(IpAddress& ip, ushort port, Socket::Type type) : Socket(ip, port, type) {}
			TcpSocket(ushort port, Socket::Type type) : Socket(port, type) {}
			void Listen(int buffer=10) {if(listen(fd,buffer)<0) throw std::runtime_error(Error("Listen"));}
			TcpSocket* Accept(); 
	};

	struct UdpSocket : public Socket
	{
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
	static void UdpSend(Command c, UdpSocket* sock, sf::IpAddress& ip, ushort port, Packet& p)
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
