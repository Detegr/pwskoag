#pragma once
#include <dtglib/Concurrency.h>
#include <Game/ServerPlayer.h>
#include <dtglib/Network.h>
#include "ThreadData.h"

namespace pwskoag
{
	typedef std::vector<std::pair<C_Thread*, C_LocalThreadData> > t_Clients;
	
	/*
	 * Server class
	 * Meant to be inherited. Includes routines for starting and
	 * stopping a server.
	 *
	 * Instantable server class must override ServerLoop(void*).
	 */
	
	class C_Server
	{
		protected:
			bool 						m_StopNow;
			uint						m_ServerPort;
			C_Mutex	 					m_SelfMutex;
			C_Thread*			 		m_SelfThread;
			static void					ServerInitializer(void* args);
			virtual void				ServerLoop()=0;
			
			PWSKOAG_API C_Server(ushort port) : m_StopNow(false), m_ServerPort(port), m_SelfThread(NULL) {};
			PWSKOAG_API	virtual	~C_Server();
		public:
			static const unsigned int TIMEOUTMS=4000;
			static const unsigned int TICK_WAITTIME_TCP=250;
			static const unsigned int TICK_WAITTIME_UDP=33;
			PWSKOAG_API virtual void 	M_Start();
			PWSKOAG_API virtual void 	M_Stop();
			PWSKOAG_API virtual void 	M_ForceStop();
			PWSKOAG_API bool			M_IsRunning() const { return m_SelfThread!=NULL; }
	};
	
	/*
	 * TcpServer class
	 *
	 * Listens Tcp-connections.
	 */
	class C_TcpServer : public C_Server
	{
		friend class UdpServer;
	private:
		C_TcpSocket 		m_TcpListener;
		C_Mutex				m_ClientLock;
		t_Clients			m_Clients;
		C_Mutex				m_PlayerLock;
		t_Entities 			m_Players;
		PWSKOAG_API void	ServerLoop();
		void				M_ParseClient(C_TcpSocket* client);
		void				M_DeleteDisconnected();
		void				M_ClearPlayers();
		void				M_GenerateId(C_TcpSocket* client);
		void				M_NewPlayer(C_TcpSocket* client);
		void				M_NewClient(C_LocalThreadData& localdata, C_Thread* thread);
	public:
		PWSKOAG_API C_TcpServer(ushort port) : C_Server(port), m_TcpListener(C_TcpSocket(port)) {}
		PWSKOAG_API ~C_TcpServer();
		const t_Clients&			M_GetClients() const {return m_Clients;}
		void						M_ClientLock(bool t) {t?m_ClientLock.M_Lock():m_ClientLock.M_Unlock();}
		void						M_PlayerLock(bool t) {t?m_PlayerLock.M_Lock():m_PlayerLock.M_Unlock();}
		t_Entities&					M_Players() {return m_Players;}
	};
	
	/*
	 * UdpServer class
	 *
	 * Listens Udp-connections.
	 */
	class C_UdpServer : public C_Server
	{
	private:
		C_TcpServer*		m_Master;
		C_UdpSocket			m_UdpSocket;
		PWSKOAG_API void	ServerLoop();
		void				M_UpdateGamestate(C_Packet& p);
	public:
		C_UdpServer(C_TcpServer* tcp, ushort port) : C_Server(port), m_Master(tcp), m_UdpSocket(C_UdpSocket(port)) {m_UdpSocket.M_Bind();}
	};
}
