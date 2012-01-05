#pragma once

#include <Util/Base.h>
#include <Concurrency/Concurrency.h>
#include <Util/Timer.h>
#include "ThreadData.h"
#include "Network_common.h"
#include <list>
#include <stdexcept>
#include <string.h>
#include <iostream>


namespace pwskoag
{
	class C_Thread; struct LocalThreadData;
	typedef std::list<std::pair<C_Thread*, LocalThreadData> > t_Clients;

	const uint TIMEOUTMS=10000;
	const uint TICKS_PER_SEC_TCP=1;
	const uint TICK_WAITTIME_TCP=250/TICKS_PER_SEC_TCP;
	const uint CONNECTTIME=3000;

	const uint TICKS_PER_SEC_UDP=33;
	const uint TICK_WAITTIME_UDP=1000/TICKS_PER_SEC_UDP;
	
	struct TcpData;
	struct LocalThreadData;
	/*
	 * TcpServer class
	 *
	 * Listens Tcp-connections.
	 */
	class TcpServer : public Server
	{	
		private:
			TcpSocket 			tcpListener;
			t_Clients			clients;
			C_Mutex				m_PlayerLock;
			std::vector<C_ServerPlayer *> m_Players;
			PWSKOAG_API void	ServerLoop();
		public:
			PWSKOAG_API TcpServer(ushort port) : Server(port), tcpListener(TcpSocket(port)) {}
			PWSKOAG_API ~TcpServer();
			const t_Clients& GetClients() const {return clients;}
	};

	class UdpServer : public Server
	{
		private:
			TcpServer*			master;
			UdpSocket			udpSocket;
			PWSKOAG_API void	ServerLoop();
		public:
			UdpServer(TcpServer* tcp, ushort port) : Server(port), master(tcp), udpSocket(UdpSocket(port)) {udpSocket.Bind();}
	};
	
	/*
	 * TcpClient class
	 */
	class TcpClient : public Client
	{
		friend class UdpClient;
		friend class C_ClientPlayer;
		friend class C_ServerPlayer;
		friend class C_Sendable;
		private:
			IpAddress			serverAddress;
			uint 				serverPort;
			TcpSocket 			tcpSocket;
			bool				m_Connected;
			C_Mutex				m_Lock;
			C_Mutex				m_ConnectMutex;
			C_Packet			packet;
			PWSKOAG_API void 	ClientLoop();
			void 				Append(e_Command c) {packet<<(uchar)c;}
			void 				Send();
			void 				Send(e_Command c) {C_Lock l(m_Lock); TcpSend(c, &tcpSocket, packet);}
			std::vector<C_ClientPlayer *> m_Players;
			C_ClientPlayer*		m_OwnPlayer;
		public:
			TcpClient() : serverAddress(), serverPort(0), tcpSocket(), m_Connected(false) {}
			PWSKOAG_API bool			M_Connect(const char* addr, ushort port);
			PWSKOAG_API void 			M_Disconnect();
			template<class type> void 	Append(e_Command c, type t) {Append(c); packet<<t;}
			const ushort				M_Id() const {return tcpSocket.M_Id();}
			std::vector<C_ClientPlayer *> M_Players() {return m_Players;}
			C_ClientPlayer*				M_OwnPlayer() {return m_OwnPlayer;}
	};

	class C_Sendable
	{
		private:
			C_Sendable() {}
		public:
			TcpSocket* m_Tcp;
			C_Packet* m_Packet;
		public:
			C_Sendable(bool t) : m_Tcp(NULL), m_Packet(NULL) {}
			C_Sendable(TcpClient* t) : m_Tcp(&t->tcpSocket), m_Packet(&t->packet) {}
			C_Sendable(TcpSocket* s, C_Packet* p) : m_Tcp(s), m_Packet(p) {}
			virtual ~C_Sendable() {}
			virtual void M_Send()=0;
	};

	class UdpClient : public Client
	{
		private:
			TcpClient*			m_Master;
			IpAddress			m_Address;
			ushort				m_Port;
			UdpSocket 			udpSocket;
			C_Packet			packet;
			C_Mutex				m_Lock;
			PWSKOAG_API void	ClientLoop();
			void 				Append(e_Command c) {packet<<(uchar)c;}
			void 				Send(IpAddress& ip, ushort port);
		public:
			UdpClient(TcpClient* t) :	m_Master(t), m_Port(0), udpSocket() {}
			PWSKOAG_API bool						M_Connect(const char* addr, ushort port);
			PWSKOAG_API void 						M_Disconnect() {udpSocket.Close(); Stop();}
			template<class type> void				Append(e_Command c, type t) {Append(c); packet<<t;}
	};
	
}
