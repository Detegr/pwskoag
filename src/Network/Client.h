#pragma once
#include <Util/Base.h>
#include <Concurrency/Concurrency.h>
#include <Network/Network.h>
#include <Game/ClientPlayer.h>

namespace pwskoag
{
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
			bool					stopNow;
			uint 					serverPort;
			C_Mutex					selfMutex;
			C_Thread*				selfThread;
			static void	ClientInitializer(void* args);
			PWSKOAG_API virtual 	~Client();
			virtual void 			ClientLoop()=0;
			
			Client() : stopNow(false), serverPort(), selfThread(NULL) {};
		public:
			PWSKOAG_API virtual void 	Start();
			PWSKOAG_API virtual void 	Stop();
			PWSKOAG_API virtual void 	ForceStop();
			virtual bool				M_Connect(const char* addr, ushort port)=0;
			virtual void				M_Disconnect()=0;
			bool 						IsRunning() const { return selfThread!=NULL; }
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
		IpAddress				serverAddress;
		uint					serverPort;
		TcpSocket				tcpSocket;
		bool					m_Connected;
		C_Mutex					m_Lock;
		C_Mutex					m_ConnectMutex;
		C_Packet				packet;
		PWSKOAG_API void		ClientLoop();
		void					Append(e_Command c) {packet<<(uchar)c;}
		void					Send();
		void					Send(e_Command c) {C_Lock l(m_Lock); TcpSend(c, &tcpSocket, packet);}
		C_Mutex					m_PlayerLock;
		std::vector<C_Player *> m_Players;
		C_ClientPlayer*			m_OwnPlayer;
	public:
		TcpClient() : serverAddress(), serverPort(0), tcpSocket(), m_Connected(false) {}
		PWSKOAG_API bool			M_Connect(const char* addr, ushort port);
		PWSKOAG_API void 			M_Disconnect();
		template<class type> void 	Append(e_Command c, type t) {Append(c); packet<<t;}
		const ushort				M_Id() const {return tcpSocket.M_Id();}
		std::vector<C_Player *>		M_Players() {return m_Players;}
		void						M_PlayerLock(bool on=true) {on?m_PlayerLock.M_Lock():m_PlayerLock.M_Unlock();}
		C_ClientPlayer*				M_OwnPlayer() {return m_OwnPlayer;}
	};
	
	/*
	 * UdpClient class
	 */
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