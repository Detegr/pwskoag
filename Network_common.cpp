#include "Network_common.h"
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
	Client::~Client()
	{
		if(selfThread) std::cerr << "!! Client not stopped and still shutting down !!" << std::endl;
	}
}
