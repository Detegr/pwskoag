#pragma once
#include <Util/Base.h>
#include <dtglib/Concurrency.h>
#include <dtglib/Network.h>
#include <Game/ClientPlayer.h>

using namespace dtglib;

namespace pwskoag
{
	typedef std::vector<C_Entity *> t_Entities;
	/*
	 * Client class
	 * Meant to be inherited. Includes routines for starting and
	 * stopping a client.
	 *
	 * Instantable client class must override ServerLoop(void*).
	 */
	class C_Client
	{
		protected:
			bool					m_StopNow;
			uint 					m_ServerPort;
			C_Mutex					m_SelfMutex;
			C_Thread*				m_SelfThread;
			static void	ClientInitializer(void* args);
			PWSKOAG_API virtual 	~C_Client();
			virtual void 			ClientLoop()=0;
			
			C_Client() : m_StopNow(false), m_ServerPort(), m_SelfThread(NULL) {};
		public:
			PWSKOAG_API virtual void 	M_Start();
			PWSKOAG_API virtual void 	M_Stop();
			PWSKOAG_API virtual void 	M_ForceStop();
			virtual bool				M_Connect(const char* addr, ushort port)=0;
			virtual void				M_Disconnect()=0;
			bool 						M_IsRunning() const { return m_SelfThread!=NULL; }
	};

	/*
	 * TcpClient class
	 */
	
	class C_TcpClient : public C_Client
	{
		friend class C_UdpClient;
		friend class C_ClientPlayer;
		friend class C_ServerPlayer;
		friend class C_Sendable;
	private:
		C_IpAddress				m_ServerAddress;
		uint					m_ServerPort;
		C_TcpSocket				m_TcpSocket;
		bool					m_Connected;
		C_Mutex					m_Lock;
		C_Mutex					m_ConnectMutex;
		C_Packet				m_Packet;
		PWSKOAG_API void		ClientLoop();
		void					M_Send();
		C_Mutex					m_PlayerLock;
		t_Entities				m_Players;
		C_ClientPlayer*			m_OwnPlayer;
	public:
		C_TcpClient() : m_ServerAddress(), m_ServerPort(0), m_TcpSocket(), m_Connected(false) {}
		PWSKOAG_API bool			M_Connect(const char* addr, ushort port);
		PWSKOAG_API void 			M_Disconnect();
		template<class type> void 	M_Append(C_Netcommand::e_Command c, type t) {m_Packet<<(uchar)c<<t;}
		const ushort				M_Id() const {return m_TcpSocket.M_Id();}
		t_Entities&					M_Players() {return m_Players;}
		void						M_PlayerLock(bool on=true) {on?m_PlayerLock.M_Lock():m_PlayerLock.M_Unlock();}
		C_Mutex&					M_GetPlayerLock() {return m_PlayerLock;}
		C_ClientPlayer*				M_OwnPlayer() {return m_OwnPlayer;}
	};
	
	/*
	 * UdpClient class
	 */
	class C_UdpClient : public C_Client
	{
		private:
			C_TcpClient*		m_Master;
			ushort				m_Port;
			C_UdpSocket 		m_UdpSocket;
			C_Packet			m_Packet;
			C_Mutex				m_Lock;
			bool				m_Initialized;
			PWSKOAG_API void	ClientLoop();
			void 				M_Send(const C_IpAddress& ip, ushort port);
		public:
			C_UdpClient() : m_Master(NULL), m_Port(0) {}
			C_UdpClient(C_TcpClient* t) : m_Master(t), m_Port(0) {m_Initialized=true;}
			void									M_Initialize(C_TcpClient* t) {m_Master=t; m_Initialized=true;}
			bool									M_Initialized() const {return m_Initialized;}
			PWSKOAG_API bool						M_Connect(const char* addr, ushort port);
			PWSKOAG_API void 						M_Disconnect() {m_UdpSocket.M_Close(); M_Stop();}
			template<class type> void				M_Append(C_Netcommand::e_Command c, type t) {m_Packet<<(uchar)c<<t;}
	};
	
}
