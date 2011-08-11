#include "Network.h"
#include <iostream>

namespace Network
{
	void TcpServer::ServerLoop()
	{
		tcpListener.Listen(serverPort);
		tcpListener.SetBlocking(false);
		sf::SocketSelector selector;
		selector.Add(tcpListener);
		while(!stopNow)
		{
			selector.Wait(10);
			if(selector.IsReady(tcpListener))
			{
				sf::TcpSocket* client = new sf::TcpSocket;
				if(tcpListener.Accept(*client) == sf::Socket::Done)
				{
					std::cout << "Client connected." << std::endl;
					clients.push_back(std::make_pair(client, sf::Clock()));
					selector.Add(*client);
				}
			}
			else
			{
				for(auto it=clients.begin(); it!=clients.end(); ++it)
				{
					sf::TcpSocket* client = it->first;
					sf::Clock& lastHeartBeat = it->second;
					if(lastHeartBeat.GetElapsedTime() > TIMEOUTMS)
					{
						it=clients.erase(it);
						std::cout << "Client timed out." << std::endl;
					}
					if(selector.IsReady(*client))
					{
						sf::Packet p;
						if(client->Receive(p)==sf::Socket::Done)
						{
							std::string data;
							p >> data;
							std::cout << data << std::endl;
						}
					}
				}
			}
		}
		tcpListener.Close();

		std::cout << "Shut down successful." << std::endl;
	}
	TcpServer::~TcpServer()
	{
		sf::Lock lock(selfMutex);
		for(auto it=clients.begin(); it!=clients.end(); ++it) delete it->first;
	}
	void TcpClient::Connect(const char* addr, ushort port)
	{
		sf::Lock lock(selfMutex);
		serverAddress=addr;
		serverPort=port;
		tcpSocket.Connect(serverAddress, serverPort);
	}
	void TcpClient::Disconnect()
	{
		sf::Lock lock(selfMutex);
		tcpSocket.Disconnect();
	}
	void TcpClient::ClientLoop() {}
}
