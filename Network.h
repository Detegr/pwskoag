#pragma once

#include <SFML/Network.hpp>
#include "Network_common.h"
#include "Base.h"
#include <list>
#include <stdexcept>
#include <string.h>
#include <iostream>

namespace Network
{
	const uint TIMEOUTMS=10000;
	const uint TICKS_PER_SEC_TCP=1;
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
			sf::TcpListener 									tcpListener;
			std::list<std::pair<TcpSocket*, sf::Clock> > 	clients;
			void												ServerLoop();
		public:
			TcpServer(ushort port) : Server(port) {}
			~TcpServer();
			const std::list<std::pair<TcpSocket*, sf::Clock> >& GetClients() const { return clients; }
	};

	class UdpServer : public Server
	{
		private:
			TcpServer*		master;
			UdpSocket		udpSocket;
			void			ServerLoop();
		public:
			UdpServer(TcpServer* tcp, ushort port) : Server(port), master(tcp)
			{udpSocket.SetBlocking(false); udpSocket.Bind(port);}
	};

	/*
	 * TcpClient class
	 */
	class TcpClient : public Client, public AutoSender
	{
		private:
			std::string			serverAddress;
			uint 				serverPort;
			Mutex				canAppend;
			TcpSocket 			tcpSocket;
			Packet				packet;
			void 				ClientLoop();
			void				AutoSendLoop();
		public:
			TcpClient() : serverAddress(), serverPort(0), tcpSocket() {}
			void 						Connect(const char* addr, ushort port);
			void 						Disconnect();
			void 						Append(Command c) {sf::Lock l(canAppend); packet<<(uchar)c;}
			template<class type> void 	Append(Command c, type t) {sf::Lock l(canAppend); Append(c); packet<<t;}
			void 						Send() {sf::Lock l(canAppend); Append(Command::EOP);tcpSocket.Send(packet); packet.Clear();}
			void 						Send(Command c) {sf::Lock l(canAppend); Network::TcpSend(c, &tcpSocket, packet);}
			bool						IsSent() const {return packet.EndOfPacket();}
	};

	class UdpClient : public Client
	{
		private:
			sf::IpAddress 	serverAddress;
			ushort			serverPort;
			sf::UdpSocket 	udpSocket;
			sf::Packet		packet;
			void			ClientLoop();
		public:
			UdpClient() : serverAddress(), serverPort(0), udpSocket() {}
			void Connect(const char* addr, ushort port);
			void Disconnect() {udpSocket.Unbind(); Stop();}
	};

	class Networking
	{
		private:
			TcpClient tcpData;
			//UdpClient UdpData;
		public:
	};
	
}
