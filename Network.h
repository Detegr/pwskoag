#pragma once

#include <SFML/Network.hpp>
#include "Base.h"

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
			bool StopNow;
			uint ServerPort;
			sf::Mutex SelfMutex;
			sf::Thread* SelfThread;
			static void ServerInitializer(void* args);

			Server(ushort port, bool start=false) : StopNow(false), ServerPort(port), SelfThread(NULL) {if(start) Start();};
			virtual ~Server();
		public:
			virtual void Start();
			virtual void Stop();
			virtual void ForceStop();
			bool isRunning() const { return SelfThread!=NULL; }

			virtual void ServerLoop()=0;
	};

	/*
	 * TcpServer class
	 *
	 * Listens Tcp-connections.
	 */
	class TcpServer : public Server
	{	
		private:
			sf::TcpListener TcpListener;
			void ServerLoop();
		public:
			TcpServer(ushort port, bool start=false) : Server(port, start) {}
	};

	/*
	 * Client class
	 * Handles threads
	 */
	class Client
	{
		protected:
			uint ServerPort;
			sf::Mutex SelfMutex;
			sf::Thread* SelfThread;
			static void ClientInitializer(void* args);

			Client() : ServerPort(), SelfThread(NULL) {};
			virtual ~Client();
		public:
			virtual void Start();
			virtual void Stop();
			virtual void ForceStop();
			virtual void Connect(const char* addr, ushort port)=0;
			virtual void Disconnect()=0;
			bool isRunning() const { return SelfThread!=NULL; }
			virtual void ClientLoop()=0;
	};

	/*
	 * TcpClient class
	 */
	class TcpClient : public Client
	{
		private:
			std::string ServerAddress;
			uint ServerPort;
			sf::TcpSocket TcpSocket;
			void ClientLoop();
		public:
			TcpClient() : ServerAddress(), ServerPort(0), TcpSocket() {}
			void Connect(const char* addr, ushort port);
			void Disconnect();
	};
}
