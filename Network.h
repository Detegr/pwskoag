#pragma once

#include <SFML/Network.hpp>
#include "Base.h"
#include <list>

namespace Network
{
	const uint TIMEOUTMS=10000;
	const uint TICKS_PER_SEC=33;
	const uint TICK_WAITTIME=1000/TICKS_PER_SEC;

	enum Command
	{
		Heartbeat,
		Connect,
		Disconnect
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
			bool 		stopNow;
			uint 		serverPort;
			sf::Mutex 	selfMutex;
			sf::Thread* 	selfThread;
			static void 	ServerInitializer(void* args);
			virtual 	~Server();
			virtual 	void ServerLoop()=0;

			Server(ushort port) : stopNow(false), serverPort(port), selfThread(NULL) {};
		public:
			virtual void 	Start();
			virtual void 	Stop();
			virtual void 	ForceStop();
			bool IsRunning() const { return selfThread!=NULL; }
	};

	/*
	 * TcpServer class
	 *
	 * Listens Tcp-connections.
	 */
	class TcpServer : public Server
	{	
		private:
			sf::TcpListener 					tcpListener;
			std::list<std::pair<sf::TcpSocket*, sf::Clock> > 	clients;
			void							ServerLoop();
		public:
			TcpServer(ushort port) : Server(port) {}
			~TcpServer();
			std::list<std::pair<sf::TcpSocket*, sf::Clock> >& GetClients() { return clients; }
	};

	/*
	 * Client class
	 * Handles threads
	 */
	class Client
	{
		protected:
			uint 		serverPort;
			sf::Mutex	selfMutex;
			sf::Thread*	selfThread;
			static void	ClientInitializer(void* args);
			virtual 	~Client();
			virtual void 	ClientLoop()=0;

			Client() : serverPort(), selfThread(NULL) {};
		public:
			virtual void 	Start();
			virtual void 	Stop();
			virtual void 	ForceStop();
			virtual void 	Connect(const char* addr, ushort port)=0;
			virtual void 	Disconnect()=0;
			bool 		IsRunning() const { return selfThread!=NULL; }
	};

	/*
	 * TcpClient class
	 */
	class TcpClient : public Client
	{
		private:
			std::string	serverAddress;
			uint 		serverPort;
			sf::TcpSocket 	tcpSocket;
			void 		ClientLoop();
		public:
			TcpClient() : serverAddress(), serverPort(0), tcpSocket() {}
			void Connect(const char* addr, ushort port);
			void Disconnect();
			void Send(sf::Packet& p) { tcpSocket.Send(p); }
	};

	class Networking
	{
		private:
			TcpClient tcpData;
			//UdpClient UdpData;
		public:
	};
}
