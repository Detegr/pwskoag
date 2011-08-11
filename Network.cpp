#include "Network.h"
#include <iostream>

namespace Network
{
	void Server::Start()
	{
		sf::Lock lock(selfMutex);
		if(!selfThread)
		{
			stopNow=false;
			selfThread = new sf::Thread(Server::ServerInitializer, this);
			selfThread->Launch();
		}
		else std::cerr << "Server already running!" << std::endl;
	}
	void Server::Stop()
	{
		sf::Lock lock(selfMutex);
		stopNow=true;
		if(selfThread) {delete selfThread; selfThread=NULL;}
		else std::cerr << "Server already stopped!" << std::endl;
	}
	void Server::ForceStop()
	{
		sf::Lock lock(selfMutex);
		if(selfThread) {selfThread->Terminate(); delete selfThread; selfThread=NULL;}
	}
	void Server::ServerInitializer(void* args)
	{
		Server* s = (Server*)args;
		s->ServerLoop();
	}
	Server::~Server()
	{
		if(selfThread) std::cerr << "!! Server not stopped and still shutting down !!" << std::endl;
	}
	TcpServer::~TcpServer()
	{
		sf::Lock lock(selfMutex);
		for(auto it=clients.begin(); it!=clients.end(); ++it) delete it->first;
	}
	void TcpServer::ServerLoop()
	{
		tcpListener.Listen(serverPort);
		tcpListener.SetBlocking(false);
		sf::SocketSelector selector;
		selector.Add(tcpListener);
		while(!stopNow)
		{
			selector.Wait(1000);
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

	Client::~Client()
	{}
	void Client::Start()
	{
		sf::Lock lock(selfMutex);
		if(!selfThread)
		{
			selfThread = new sf::Thread(Client::ClientInitializer, this);
			selfThread->Launch();
		}
		else std::cerr << "Client already running!" << std::endl;
	}
	void Client::Stop()
	{
		sf::Lock lock(selfMutex);
		if(selfThread) {selfThread->Wait(); delete selfThread; selfThread=NULL;}
		else std::cerr << "Client already stopped!" << std::endl;
	}
	void Client::ForceStop()
	{
		sf::Lock lock(selfMutex);
		if(selfThread) {selfThread->Terminate(); delete selfThread; selfThread=NULL;}
	}
	void Client::ClientInitializer(void* args)
	{
		Client* s = (Client*)args;
		s->ClientLoop();
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
