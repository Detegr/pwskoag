#pragma once

#include <SFML/Network.hpp>
#include "Base.h"
#include <list>

namespace Network
{
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
			bool 		StopNow;
			uint 		ServerPort;
			sf::Mutex 	SelfMutex;
			sf::Thread* 	SelfThread;
			static void 	ServerInitializer(void* args);
			virtual 	~Server();
			virtual 	void ServerLoop()=0;

			Server(ushort port) : StopNow(false), ServerPort(port), SelfThread(NULL) {};
		public:
			virtual void Start();
			virtual void Stop();
			virtual void ForceStop();
			bool isRunning() const { return SelfThread!=NULL; }
	};

	/*
	 * TcpServer class
	 *
	 * Listens Tcp-connections.
	 */
	class TcpServer : public Server
	{	
		private:
			sf::TcpListener 		TcpListener;
			std::list<sf::TcpSocket*> 	Clients;
			void				ServerLoop();
		public:
			TcpServer(ushort port) : Server(port) {}
			~TcpServer();
			std::list<sf::TcpSocket*>& GetClients() { return Clients; }
	};

	/*
	 * Client class
	 * Handles threads
	 */
	class Client
	{
		protected:
			uint 		ServerPort;
			sf::Mutex	SelfMutex;
			sf::Thread*	SelfThread;
			static void	ClientInitializer(void* args);
			virtual 	~Client();
			virtual void 	ClientLoop()=0;

			Client() : ServerPort(), SelfThread(NULL) {};
		public:
			virtual void 	Start();
			virtual void 	Stop();
			virtual void 	ForceStop();
			virtual void 	Connect(const char* addr, ushort port)=0;
			virtual void 	Disconnect()=0;
			bool 		isRunning() const { return SelfThread!=NULL; }
	};

	/*
	 * TcpClient class
	 */
	class TcpClient : public Client
	{
		private:
			std::string	ServerAddress;
			uint 		ServerPort;
			sf::TcpSocket 	TcpSocket;
			void 		ClientLoop();
		public:
			TcpClient() : ServerAddress(), ServerPort(0), TcpSocket() {}
			void Connect(const char* addr, ushort port);
			void Disconnect();
			void Send(sf::Packet& p) { TcpSocket.Send(p); }
	};

	class Networking
	{
		private:
			TcpClient TcpData;
			//UdpClient UdpData;
		public:
	};
}
