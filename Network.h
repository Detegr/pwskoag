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
			void 				Connect(const char* addr, ushort port);
			void 				Disconnect();
			void 				Append(Command c) {packet<<(uchar)c;}
			template<class type> void 	Append(Command c, type t) {Append(c); packet<<t;}
			void 				Send() {Append(Command::EOP);tcpSocket.Send(packet); packet.Clear();}
			void 				Send(Command c) {Network::TcpSend(c, &tcpSocket, packet);}
	};

	class UdpClient : public Client
	{
		private:
			std::string 	serverAddress;
			uint			serverPort;
			sf::UdpSocket 	udpSocket;
			sf::Packet		packet;
			void			ClientLoop();
		public:
			UdpClient() : serverAddress(), serverPort(0), udpSocket() {}
			void SetServer(const char* addr, uint port);
			void Clear() {udpSocket.Unbind();}
	};

	class Networking
	{
		private:
			TcpClient tcpData;
			//UdpClient UdpData;
		public:
	};
	
}
