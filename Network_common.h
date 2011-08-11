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
	 * Client class
	 * Meant to be inherited. Includes routines for starting and
	 * stopping a client.
	 *
	 * Instantable client class must override ServerLoop(void*).
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

}
