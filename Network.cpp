#include "Network.h"
#include <iostream>

namespace Network
{
	void Server::Start()
	{
		sf::Lock lock(SelfMutex);
		if(!SelfThread)
		{
			StopNow=false;
			SelfThread = new sf::Thread(Server::ServerInitializer, this);
			SelfThread->Launch();
		}
		else std::cerr << "Server already running!" << std::endl;
	}
	void Server::Stop()
	{
		sf::Lock lock(SelfMutex);
		StopNow=true;
		if(SelfThread) {delete SelfThread; SelfThread=NULL;}
		else std::cerr << "Server already stopped!" << std::endl;
	}
	void Server::ForceStop()
	{
		sf::Lock lock(SelfMutex);
		if(SelfThread) {SelfThread->Terminate(); delete SelfThread; SelfThread=NULL;}
	}
	void Server::ServerInitializer(void* args)
	{
		Server* s = (Server*)args;
		s->ServerLoop();
	}
	Server::~Server()
	{
		if(SelfThread) std::cerr << "!! Server not stopped and still shutting down !!" << std::endl;
	}
	TcpServer::~TcpServer()
	{
		sf::Lock lock(SelfMutex);
		for(auto it=Clients.begin(); it!=Clients.end(); ++it) delete *it;
	}
	void TcpServer::ServerLoop()
	{
		TcpListener.Listen(ServerPort);
		TcpListener.SetBlocking(false);
		sf::SocketSelector selector;
		selector.Add(TcpListener);
		while(!StopNow)
		{
			selector.Wait(1000);
			if(selector.IsReady(TcpListener))
			{
				sf::TcpSocket* client = new sf::TcpSocket;
				if(TcpListener.Accept(*client) == sf::Socket::Done)
				{
					std::cout << "Client connected." << std::endl;
					Clients.push_back(client);
					selector.Add(*client);
				}
			}
			else
			{
				for(auto it=Clients.begin(); it!=Clients.end(); ++it)
				{
					sf::TcpSocket& client = **it;
					if(selector.IsReady(client))
					{
						sf::Packet p;
						if(client.Receive(p)==sf::Socket::Done)
						{
							std::string data;
							p >> data;
							std::cout << data << std::endl;
						}
					}
				}
			}
		}
		TcpListener.Close();
		std::cout << "Shut down successful." << std::endl;
	}

	Client::~Client()
	{
		sf::Lock lock(SelfMutex);
		if(SelfThread) Stop();
	}
	void Client::Start()
	{
		sf::Lock lock(SelfMutex);
		if(!SelfThread)
		{
			SelfThread = new sf::Thread(Client::ClientInitializer, this);
			SelfThread->Launch();
		}
		else std::cerr << "Client already running!" << std::endl;
	}
	void Client::Stop()
	{
		sf::Lock lock(SelfMutex);
		if(SelfThread) {SelfThread->Wait(); delete SelfThread; SelfThread=NULL;}
		else std::cerr << "Client already stopped!" << std::endl;
	}
	void Client::ForceStop()
	{
		sf::Lock lock(SelfMutex);
		if(SelfThread) {SelfThread->Terminate(); delete SelfThread; SelfThread=NULL;}
	}
	void Client::ClientInitializer(void* args)
	{
		Client* s = (Client*)args;
		s->ClientLoop();
	}
	void TcpClient::Connect(const char* addr, ushort port)
	{
		sf::Lock lock(SelfMutex);
		ServerAddress=addr;
		ServerPort=port;
		TcpSocket.Connect(ServerAddress, ServerPort);
	}
	void TcpClient::Disconnect()
	{
		sf::Lock lock(SelfMutex);
		TcpSocket.Disconnect();
	}
	void TcpClient::ClientLoop() {}
}
