#pragma once

#include <SFML/Network.hpp>
#include "Network_common.h"
#include "Base.h"
#include <list>

namespace Network
{
	const uint TIMEOUTMS=10000;
	const uint TICKS_PER_SEC_TCP=4;
	const uint TICK_WAITTIME_TCP=1000/TICKS_PER_SEC_TCP;

	const uint TICKS_PER_SEC_UDP=33;
	const uint TICK_WAITTIME_UDP=1000/TICKS_PER_SEC_UDP;
	
	enum Command
	{
		Heartbeat,
		Connect,
		Disconnect,
		EOP=255
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
	 * TcpClient class
	 */
	class TcpClient : public Client
	{
		private:
			std::string	serverAddress;
			uint 		serverPort;
			sf::TcpSocket 	tcpSocket;
			sf::Packet	packet;
			void 		ClientLoop();
		public:
			TcpClient() : serverAddress(), serverPort(0), tcpSocket() {}
			void Connect(const char* addr, ushort port);
			void Disconnect();
			void Send(sf::Packet& p) {tcpSocket.Send(p); packet.Clear();}
			void Send(Command c);
	};

	class Networking
	{
		private:
			TcpClient tcpData;
			//UdpClient UdpData;
		public:
	};
}
