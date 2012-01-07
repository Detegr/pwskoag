#pragma once
#include <Concurrency/Concurrency.h>
#include <Game/ServerPlayer.h>
#include "Network.h"
#include "ThreadData.h"
#include <list>

namespace pwskoag
{
	typedef std::list<std::pair<C_Thread*, LocalThreadData> > t_Clients;
	
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
			bool 						stopNow;
			uint			 			serverPort;
			C_Mutex	 					selfMutex;
			C_Thread*			 		selfThread;
			static void					ServerInitializer(void* args);
			virtual void				ServerLoop()=0;
			
			PWSKOAG_API Server(ushort port) : stopNow(false), serverPort(port), selfThread(NULL) {};
			PWSKOAG_API	virtual	~Server();
		public:
			PWSKOAG_API virtual void 	Start();
			PWSKOAG_API virtual void 	Stop();
			PWSKOAG_API virtual void 	ForceStop();
			PWSKOAG_API bool IsRunning() const { return selfThread!=NULL; }
	};
	
	/*
	 * TcpServer class
	 *
	 * Listens Tcp-connections.
	 */
	class TcpServer : public Server
	{	
	private:
		TcpSocket 			m_TcpListener;
		t_Clients			m_Clients;
		C_Mutex				m_PlayerLock;
		std::vector<C_Player *> m_Players;
		PWSKOAG_API void	ServerLoop();
		void				M_ParseClient(TcpSocket* client);
		void				M_DeleteDisconnected();
		void				M_ClearPlayers();
	public:
		PWSKOAG_API TcpServer(ushort port) : Server(port), m_TcpListener(TcpSocket(port)) {}
		PWSKOAG_API ~TcpServer();
		const t_Clients& GetClients() const {return m_Clients;}
	};
	
	/*
	 * UdpServer class
	 *
	 * Listens Udp-connections.
	 */
	class UdpServer : public Server
	{
	private:
		TcpServer*			master;
		UdpSocket			udpSocket;
		PWSKOAG_API void	ServerLoop();
	public:
		UdpServer(TcpServer* tcp, ushort port) : Server(port), master(tcp), udpSocket(UdpSocket(port)) {udpSocket.Bind();}
	};
}