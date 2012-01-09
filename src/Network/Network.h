#pragma once
#include <Util/Base.h>
#include <Concurrency/Concurrency.h>
#include "Packet.h"

#include <algorithm>
#include <vector>
#include <stdexcept>
#include <string.h>
#include <fcntl.h>

#ifdef _WIN32
	#pragma warning( disable : 4800 )
#else
	#include <arpa/inet.h>
#endif

namespace pwskoag
{
	
	const uint TIMEOUTMS=10000;
	const uint TICKS_PER_SEC_TCP=1;
	const uint TICK_WAITTIME_TCP=250/TICKS_PER_SEC_TCP;
	const uint CONNECTTIME=60000;
	
	const uint TICKS_PER_SEC_UDP=33;
	const uint TICK_WAITTIME_UDP=1000/TICKS_PER_SEC_UDP;
	
	#ifdef _WIN32
		class C_SocketInitializer
		{
			private:
				WSADATA m_Data;
			public:
				C_SocketInitializer()
				{
					if(WSAStartup(MAKEWORD(2,2), &m_Data)!=0)
					{
						std::runtime_error("Couldn't find Winsock DLL");
						exit(1);
					}
				}
				~C_SocketInitializer()
				{
					WSACleanup();
				}
		};
	#endif

	class IpAddress
	{
		friend class Socket;
		friend class TcpSocket;
		friend struct UdpSocket;
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
			PWSKOAG_API Socket(IpAddress& ip, ushort port, Type type);
			PWSKOAG_API Socket(ushort port, Type type);
			PWSKOAG_API Socket(const Socket& s) {*this=s;}
			PWSKOAG_API virtual ~Socket() {}
			PWSKOAG_API const Socket& operator=(const Socket& s) {ip=s.ip;port=s.port;fd=s.fd;type=s.type;addr=s.addr; return *this;}
			PWSKOAG_API void Bind();
			PWSKOAG_API void Close()
			{
				#ifdef _WIN32
					closesocket(fd);
				#else
					shutdown(fd, SHUT_RDWR);
					close(fd);
				#endif
			}
			PWSKOAG_API const IpAddress&	GetIp() const {return ip;}
			PWSKOAG_API const ushort		GetPort() const {return port;}
			PWSKOAG_API const ushort		M_Id() const {return m_Id;}
			PWSKOAG_API void				M_Id(ushort id) {m_Id=id;}
			PWSKOAG_API bool 				M_Closed() const {return this->fd<0;}
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
			bool Send(C_Packet& p); 
			bool Receive(C_Packet& p);
			void M_UdpPort(ushort port) {m_UdpPort=port;}
			const ushort M_UdpPort() const {return m_UdpPort;}
	};

	struct UdpSocket : public Socket
	{
		UdpSocket() {}
		UdpSocket(IpAddress& ip, ushort port) : Socket(ip, port, UDP) {}
		UdpSocket(ushort port) : Socket(port, UDP) {}
		bool Send(C_Packet& p, IpAddress& ip, ushort port);
		bool Receive(C_Packet& p, IpAddress* ip=NULL, ushort* port=NULL);
	};

	class Selector
	{
		private:
			std::vector<int> fd_ints;
			fd_set fds;
		public:
			Selector() {FD_ZERO(&fds);}
			void Add(Socket& s) {fd_ints.push_back(s.fd); std::sort(fd_ints.begin(), fd_ints.end());}
			bool IsReady(Socket& s) {if(s.fd!=-1) return FD_ISSET(s.fd, &fds); else return false;}
			void Remove(Socket& s);
			void Clear() {fd_ints.clear(); FD_ZERO(&fds);}
			int Wait(uint timeoutms);
			int WaitWrite(uint timeoutms);
			int WaitReadWrite(uint timeoutms);
	};

	// Functions for sending and appending.
	static void Append(e_Command c, C_Packet& p) {p<<(uchar)c;}
	template <class type> void Append(type& t, C_Packet& p){p<<t;}
	template <class type> void Append(e_Command c, type& t, C_Packet& p){Append(c,p);Append(t,p);}

	// Tcp-functions
	static bool TcpSend(e_Command c, TcpSocket* sock, C_Packet& p)
	{
		p << (uchar)c << (uchar)EOP;
		return sock->Send(p);
	}
	static bool TcpSend(TcpSocket* sock, C_Packet& p) {return sock->Send(p);}
	template <class type> bool TcpSend(e_Command c, type t, TcpSocket* sock, C_Packet& p)
	{
			p.M_Clear();
			Append(c,t,p); Append(EOP, p);
			return TcpSend(sock,p);
	}

	// Udp-Functions
	static bool UdpSend(UdpSocket* sock, C_Packet& p, IpAddress& ip, ushort port) {return sock->Send(p, ip, port);}
	template <class type>
	bool UdpSend(e_Command c, type t, UdpSocket* sock, C_Packet& p, IpAddress& ip, ushort port)
	{
		p.M_Clear();
		Append(c, t, p); Append(EOP, p);
		return UdpSend(sock, p, ip, port);
	}
	
	class C_Sendable
	{
		private:
		public:
			TcpSocket* m_Tcp;
			C_Packet* m_Packet;
			C_Sendable() : m_Tcp(NULL), m_Packet(NULL) {}
			C_Sendable(TcpSocket* s, C_Packet* p) : m_Tcp(s), m_Packet(p) {}
			virtual ~C_Sendable() {}
			virtual void M_Send()=0;
	};

}
