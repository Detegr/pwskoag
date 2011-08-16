#pragma once
#include <SFML/Network.hpp>
#include <unordered_map>
#include "Base.h"
#include "Network_commands.h"

namespace Network
{
	// Functions for sending and appending.
	static void Append(Command c, sf::Packet& p) {p<<(uchar)c;}
	template <class type> void Append(type t, sf::Packet& p){p<<t;}
	template <class type> void Append(Command c, type t, sf::Packet& p){Append(c,p);p<<t;}

	// Tcp-functions
	static void TcpSend(Command c, sf::TcpSocket* sock, sf::Packet& p)
	{
		p << (uchar)c << (uchar)Command::EOP;
		sock->Send(p);
		p.Clear();
	}
	static void TcpSend(sf::TcpSocket* sock, sf::Packet& p) {sock->Send(p); p.Clear();}
	template <class type> void TcpSend(Command c, type t, sf::TcpSocket* sock, sf::Packet& p)
	{
			p.Clear();
			Append(c,t,p); Append(Command::EOP, p); TcpSend(sock,p);
	}

	// Udp-functions
	static void UdpSend(Command c, sf::UdpSocket* sock, sf::IpAddress& ip, ushort port, sf::Packet& p)
	{
		p << (uchar)c << (uchar)Command::EOP;
		sock->Send(p, ip, port);
	}
	static void UdpSend(sf::UdpSocket* sock, sf::IpAddress& ip, ushort port, sf::Packet& p) {sock->Send(p,ip,port); p.Clear();}
	template <class type>
	void UdpSend(Command c, type t, sf::UdpSocket* sock, sf::IpAddress& ip, ushort port, sf::Packet& p)
	{
		p.Clear();
		Append(c, t, p); Append(Command::EOP, p); UdpSend(sock, ip, port, p);
	}

	class AutoSender
	{
		protected:
			bool								stopNow;
			sf::Thread*							selfThread;
			sf::Mutex							autoSendMutex;
			sf::Mutex							selfMutex;
			static void							AutoSendInitializer(void* args);
			std::unordered_map<uchar, void*>	objectsToSend;
			virtual void						AutoSendLoop()=0;
			void								Start();
			void								Stop();
			void								ForceStop();
			AutoSender() : stopNow(false), selfThread(NULL) {}
		public:
			template <class type> void AutoSend(Command c, type* t)
			{
				sf::Lock lock(autoSendMutex);
				objectsToSend.insert(std::make_pair((uchar)c,t));
			}
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
			bool 			stopNow;
			uint 			serverPort;
			sf::Mutex 		selfMutex;
			sf::Thread* 	selfThread;
			static void 	ServerInitializer(void* args);
			virtual 		~Server();
			virtual void	ServerLoop()=0;

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
			bool			stopNow;
			uint 			serverPort;
			sf::Mutex		selfMutex;
			sf::Thread*		selfThread;
			static void		ClientInitializer(void* args);
			virtual 		~Client();
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
