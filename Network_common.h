#pragma once
#include <SFML/Network.hpp>
#include "Base.h"
#include "Network_commands.h"

namespace Network
{
	// Functions for sending and appending.
	static void Send(Command c, sf::TcpSocket* sock, sf::Packet& p)
	{
		p<<(uchar)c;
		p<<(uchar)Command::EOP;
		sock->Send(p);
		p.Clear();
	}
	static void Send(sf::TcpSocket* sock, sf::Packet& p) {sock->Send(p); p.Clear();}
	static void Append(Command c, sf::Packet& p) {p<<(uint)c;}
	template <class type> void Append(type t, sf::Packet& p){p<<t;}
	template <class type> void Append(Command c, type t, sf::Packet& p){Append(c,p);p<<t;}
	template <class type> void Send(Command c, type t, sf::TcpSocket* sock, sf::Packet& p){p.Clear();Append(c,p);Append(t,p);Append(Command::EOP, p);Send(sock,p);}

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
			bool		stopNow;
			uint 		serverPort;
			sf::Mutex	selfMutex;
			sf::Thread*	selfThread;
			static void	ClientInitializer(void* args);
			virtual 	~Client();
			virtual void 	ClientLoop()=0;

			Client() : stopNow(false), serverPort(), selfThread(NULL) {};
		public:
			virtual void 	Start();
			virtual void 	Stop();
			virtual void 	ForceStop();
			virtual void 	Connect(const char* addr, ushort port)=0;
			virtual void 	Disconnect()=0;
			bool 	     	IsRunning() const { return selfThread!=NULL; }
	};

}
