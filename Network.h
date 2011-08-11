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
		String,
		EOP=255
	};
	
	/*
	 * Static sending and appending
	 * primarily for Server to use.
	 */
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
			void 				Send(Command c) {Network::Send(c, &tcpSocket, packet);}
	};

	class Networking
	{
		private:
			TcpClient tcpData;
			//UdpClient UdpData;
		public:
	};
	
}
