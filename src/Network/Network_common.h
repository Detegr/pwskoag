#pragma once
#include <Util/Base.h>
#include <Concurrency/Concurrency.h>

#include <vector>
#include <stdexcept>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>

namespace pwskoag
{
	enum e_Command
	{
		HandShake=0,
		Heartbeat,
		Connect,
		Disconnect,
		String,
		EOP=255
	};

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
			IpAddress(struct in_addr a) {addr=a;}
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
			Mutex				m_Lock;
			std::vector<uchar>	data;
			void 				Append(const void* d, size_t len) {Lock l(m_Lock);data.resize(data.size()+len);memcpy(&data[data.size()-len], d, len);}
			void				Pop(size_t bytes) {Lock l(m_Lock);data.erase(data.begin(), data.begin()+bytes);}
		public:
			static const size_t				MAXSIZE;
			const uchar* 					RawData() const {return &data[0];}
			void 							Clear() {data.clear();}
			size_t							Size() const {return data.size();}
			Packet&							operator<<(const char* str) {Append(str, strlen(str)+1);}
			Packet&							operator<<(const std::string& str){Append(str.c_str(), str.length()+1); return *this;}
			Packet&							operator>>(char* str) {strcpy(str, (char*)&data[0]); Pop(strlen(str)+1); return *this;}
			Packet&							operator>>(std::string& str) {str=(char*)&data[0]; Pop(str.length()+1); return *this;}
			Packet&							operator<<(e_Command c) {uchar x=(uchar)c; Append(&x, sizeof(x)); return *this;}
			template <class type> Packet&	operator<<(type x) {Append(&x, sizeof(type)); return *this;}
			template <class type> Packet&	operator>>(type& x) {if(Size()){x=*(type*)&data[0]; Pop(sizeof(type));} return *this;}
	};

	class Socket
	{
		friend class Selector;
		protected:
			ushort		m_Id;
			IpAddress	ip;
			ushort		port;
			int			type;
			sockaddr_in	addr;

			Socket() : m_Id(0) {}
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
			virtual ~Socket() {}
			const Socket& operator=(const Socket& s) {ip=s.ip;port=s.port;fd=s.fd;type=s.type;addr=s.addr; return *this;}
			void Bind();
			void Close() {close(fd);}
			const IpAddress&	GetIp() const {return ip;}
			const ushort		GetPort() const {return port;}
			const ushort		M_Id() const {return m_Id;}
			void				M_Id(ushort id) {m_Id=id;}
			bool 				M_Closed() const {return this->fd<0;}
	};

	class TcpSocket : public Socket
	{
		private:
			TcpSocket(IpAddress& ip, ushort port, Type type, int fd) : Socket(ip, port, type) {m_UdpPort=0; this->fd=fd;}
			ushort m_UdpPort;
		public:
			TcpSocket() {}
			TcpSocket(IpAddress ip, ushort port) : Socket(ip, port, TCP) {}
			TcpSocket(IpAddress& ip, ushort port) : Socket(ip, port, TCP) {}
			TcpSocket(ushort port) : Socket(port, TCP) {}
			void Listen(int buffer=5) {if(listen(fd,buffer)!=0) throw std::runtime_error(Error("Listen"));}
			void Connect();
			void Disconnect() {if(fd>0)Close();}
			void Clear() {Close(); this->fd=-1;}
			TcpSocket* Accept(); 
			bool Send(Packet& p); 
			bool Receive(Packet& p);
			void M_UdpPort(ushort port) {m_UdpPort=port;}
			const ushort M_UdpPort() const {return m_UdpPort;}
	};

	struct UdpSocket : public Socket
	{
		UdpSocket() {}
		UdpSocket(IpAddress& ip, ushort port) : Socket(ip, port, UDP) {}
		UdpSocket(ushort port) : Socket(port, UDP) {}
		bool Send(Packet& p, IpAddress& ip, ushort port);
		bool Receive(Packet& p, IpAddress* ip=NULL, ushort* port=NULL);
	};

	class Selector
	{
		private:
			std::vector<int> fd_ints;
			fd_set fds;
		public:
			Selector() {FD_ZERO(&fds);}
			void Add(Socket& s) {fd_ints.push_back(s.fd); std::sort(fd_ints.begin(), fd_ints.end());}
			bool IsReady(Socket& s) {return FD_ISSET(s.fd, &fds);}
			void Remove(Socket& s);
			void Clear() {fd_ints.clear(); FD_ZERO(&fds);}
			int Wait(uint timeoutms);
			int WaitWrite(uint timeoutms);
			int WaitReadWrite(uint timeoutms);
	};

	// Functions for sending and appending.
	static void Append(e_Command c, Packet& p) {p<<(uchar)c;}
	template <class type> void Append(type& t, Packet& p){p<<t;}
	template <class type> void Append(e_Command c, type& t, Packet& p){Append(c,p);Append(t,p);}

	// Tcp-functions
	static bool TcpSend(e_Command c, TcpSocket* sock, Packet& p)
	{
		p << (uchar)c << (uchar)EOP;
		return sock->Send(p);
	}
	static bool TcpSend(TcpSocket* sock, Packet& p) {return sock->Send(p);}
	template <class type> bool TcpSend(e_Command c, type t, TcpSocket* sock, Packet& p)
	{
			p.Clear();
			Append(c,t,p); Append(EOP, p);
			return TcpSend(sock,p);
	}

	// Udp-Functions
	static bool UdpSend(UdpSocket* sock, Packet& p, IpAddress& ip, ushort port) {return sock->Send(p, ip, port);}
	template <class type>
	bool UdpSend(e_Command c, type t, UdpSocket* sock, Packet& p, IpAddress& ip, ushort port)
	{
		p.Clear();
		Append(c, t, p); Append(EOP, p);
		return UdpSend(sock, p, ip, port);
	}

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
			Mutex	 		selfMutex;
			Thread* 		selfThread;
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
			Mutex			selfMutex;
			Thread*			selfThread;
			static void		ClientInitializer(void* args);
			virtual 		~Client();
			virtual void 	ClientLoop()=0;

			Client() : stopNow(false), serverPort(), selfThread(NULL) {};
		public:
			virtual void 	Start();
			virtual void 	Stop();
			virtual void 	ForceStop();
			virtual bool	M_Connect(const char* addr, ushort port)=0;
			virtual void 	M_Disconnect()=0;
			bool 	     	IsRunning() const { return selfThread!=NULL; }
	};
}