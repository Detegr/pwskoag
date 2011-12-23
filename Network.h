#pragma once

#include "Network_common.h"
#include "Base.h"
#include "Concurrency.h"
#include "Timer.h"
#include <list>
#include <stdexcept>
#include <string.h>
#include <iostream>


namespace pwskoag
{
	class Thread; class LocalThreadData;
	typedef std::list<std::pair<Thread*, LocalThreadData> > t_Clients;

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
			TcpSocket 	tcpListener;
			t_Clients	clients;
			void		ServerLoop();
		public:
			TcpServer(ushort port) : Server(port), tcpListener(TcpSocket(port)) {}
			~TcpServer();
			const t_Clients& GetClients() const {return clients;}
	};

	class UdpServer : public Server
	{
		private:
			TcpServer*		master;
			UdpSocket		udpSocket;
			void			ServerLoop();
		public:
			UdpServer(TcpServer* tcp, ushort port) : Server(port), master(tcp), udpSocket(UdpSocket(port)) {udpSocket.Bind();}
	};
	
	struct ThreadData
	{
		Mutex* lock;
		TcpSocket*			socket;
		C_Timer*			timer;
		bool*				stopNow;
		ThreadData(Mutex *l, C_Timer* t, TcpSocket* sock, bool* stop) :
			lock(l), timer(t), socket(sock), stopNow(stop) {}
	};

	struct LocalThreadData
	{
		TcpSocket*			socket;
		C_Timer				timer;
		Mutex	lock;
		LocalThreadData(TcpSocket* s) : socket(s), timer(C_Timer()) {}
	};

	/*
	 * TcpClient class
	 */
	class TcpClient : public Client
	{
		friend class UdpClient;
		private:
			IpAddress			serverAddress;
			uint 				serverPort;
			TcpSocket 			tcpSocket;
			bool				m_Connected;
			Mutex				canAppend;
			Mutex				m_ConnectMutex;
			Packet				packet;
			void 				ClientLoop();
		public:
			TcpClient() : serverAddress(), serverPort(0), tcpSocket(), m_Connected(false) {}
			bool 						M_Connect(const char* addr, ushort port);
			void 						M_Disconnect();
			void 						Append(e_Command c) {Lock l(canAppend); packet<<(uchar)c;}
			template<class type> void 	Append(e_Command c, type t) {Lock l(canAppend); Append(c); packet<<t;}
			void 						Send() {Lock l(canAppend); Append(EOP);tcpSocket.Send(packet); packet.Clear();}
			void 						Send(e_Command c) {Lock l(canAppend); TcpSend(c, &tcpSocket, packet);}
			bool						IsSent() const {return packet.Size()==0;}
			int							DataSize() const {return packet.Size();}
	};

	class UdpClient : public Client
	{
		private:
			IpAddress 		serverAddress;
			ushort			serverPort;
			UdpSocket 		udpSocket;
			Packet			packet;
			void			ClientLoop();
		public:
			UdpClient() : serverAddress(), serverPort(0), udpSocket() {}
			bool M_Connect(const char* addr, ushort port);
			void M_Disconnect() {udpSocket.Close(); Stop();}
	};

	class Networking
	{
		private:
			TcpClient tcpData;
			//UdpClient UdpData;
		public:
	};
	
}
