#include "Network.h"
#include <iostream>

namespace Network
{
	void ReceiveThread(void *args)
	{
		ThreadData* data=(ThreadData*)args;
		Concurrency::Mutex* lock=data->lock;
		sf::Clock* timer=data->timer;
		TcpSocket* client=data->socket;
		bool* stopNow=data->stopNow;
		delete data;

		fd_set set;

		while(!*stopNow)
		{
			Packet p;
			FD_SET(client->fd, &set);
			int recv=select(client->fd+1, &set, nullptr, nullptr, nullptr);
			if(recv)
			{
				if(client->Receive(p))
				{
					FD_CLR(client->fd, &set);
					uchar header=0;
					while(p.Size())
					{
						p>>header;
						switch (header)
						{
							case Command::String:
								{
									std::string str;
									p>>str;
									std::cout << "Str: " << str << std::endl;
									break;
								}
							case Command::Heartbeat:
								lock->Lock();
								timer->Reset();
								lock->Unlock();
								std::cout << "Beat from " << client->GetIp() << ":" << client->GetPort() << std::endl;
								break;
							case Command::Disconnect:		
								lock->Lock();
								client->Clear();
								delete client;
								lock->Unlock();
								client=NULL;
								std::cout << "Client disconnected." << std::endl;
								pthread_exit(0);
								break;
							case Command::EOP: goto EndOfPacket;
							default: break;
						}
					}
					EndOfPacket:
					Packet toClient;
					std::string str("Hi, this is server speaking.");
					if(!TcpSend(Command::String, str, client, toClient))
					{
						delete client;
						client=NULL;
						std::cout << "Client disconnected: Terminating the connection." << std::endl;
						pthread_exit(0);
					}
					std::cout << "Sent: " << str << std::endl;
				}
			}
		}
	}

	void TcpServer::ServerLoop()
	{
		tcpListener.Bind();
		tcpListener.Listen();
		tcpListener.SetBlocking(true);
		Selector selector;
		selector.Add(tcpListener);
		while(!stopNow)
		{
			bool data=selector.Wait(TICK_WAITTIME_TCP);
			if(selector.IsReady(tcpListener))
			{
				TcpSocket* client = tcpListener.Accept();
				if(client)
				{
					Packet p;
					if(client->Receive(p))
					{
						uchar header; p>>header;
						if(header==Command::Connect)
						{
							std::cout << "Client connected" << std::endl;
							clients.push_back(std::make_pair(nullptr, LocalThreadData(client)));
							ThreadData* data=new ThreadData(&clients.back().second.lock, &clients.back().second.timer, client, &stopNow);
							Concurrency::Thread* run=new Concurrency::Thread(ReceiveThread, (void*)data);
							clients.back().first=run;
						}
					}
				}
			}
			
			for(auto it=clients.begin(); it!=clients.end(); ++it)
			{
				if(!it->second.socket->Initialized())
				{
					it->second.lock.Lock();
					std::cout << "Removed disconnected client from clients." << std::endl;
					delete it->first;
					it->second.lock.Unlock();
					it=clients.erase(it);
					continue;
				}
				it->second.lock.Lock();
				uint lastheartbeat=it->second.timer.GetElapsedTime();
				it->second.lock.Unlock();
				if(lastheartbeat>8000)
				{
					std::cout << "Client " << it->second.socket->GetIp() << " timed out." << std::endl;
					it->first->Join();
					delete it->first;
					it=clients.erase(it);
				}
			}
		}
		tcpListener.Close();

		if(clients.size()>0) {std::cout << "There were " << clients.size() << " clients connected." << std::endl;}
		std::cout << "Shut down successful." << std::endl;
	}
	void UdpServer::ServerLoop()
	{
		/*
		Packet p;
		while(!stopNow)
		{
			auto clients = master->GetClients();
			for(auto it=clients.begin(); it!=clients.end(); ++it)
			{
				p.Clear();
				IpAddress ip = it->first->GetIp();
				ushort port = it->first->GetPort();
				if(udpSocket.Receive(p, ip, port))
				{
					std::cout << "Got data from: " << it->first->GetIp() << std::endl;
				}
			}
			msSleep(TICK_WAITTIME_UDP);
		}
		*/
	}
		
	TcpServer::~TcpServer()
	{
		Concurrency::Lock lock(selfMutex);
		for(auto it=clients.begin(); it!=clients.end(); ++it) delete it->first;
	}
	void TcpClient::Connect(const char* addr, ushort port)
	{
		Concurrency::Lock lock(Client::selfMutex);
		serverAddress=addr;
		serverPort=port;
		tcpSocket=TcpSocket(IpAddress(addr), port);
		tcpSocket.SetBlocking(true);
		tcpSocket.Connect();
		Send(Command::Connect);
		Client::Start();
		//AutoSender::Start();
	}
	void TcpClient::Disconnect()
	{
		Send(Command::Disconnect);
		Client::Stop();
		//AutoSender::Stop();
		Concurrency::Lock lock(Client::selfMutex);
		tcpSocket.Disconnect();
	}

	void ReceiveThread_Client(void *args)
	{
		ThreadData* data=(ThreadData*)args;
		TcpSocket* tcpSocket=data->socket;
		bool* stopNow=data->stopNow;
		fd_set set;
		struct timeval tv;
		while(!*stopNow)
		{
			Packet p;
			uchar header=0;
			bool end=false;
			FD_SET(tcpSocket->fd, &set);
			tv.tv_sec=1;
			tv.tv_usec=0;
			int recv=select(tcpSocket->fd+1, &set, nullptr, nullptr, &tv);
			if(recv)
			{
				if(tcpSocket->Receive(p))
				{
					FD_CLR(tcpSocket->fd, &set);
					for(;;)
					{
						p>>header;
						switch (header)
						{
							case Command::String:
							{
								std::string str;
								p >> str;
								std::cout << str << std::endl;
								break;
							}
							case Command::EOP:
								end=true;
								break;
							default:
								std::cout << "Invalid packet. Terminating." << std::endl; break;
						}
						if(end) break;
					}
				}
			}
		}
	}

	void TcpClient::ClientLoop()
	{
		sf::Clock timer;
		ThreadData* data=new ThreadData(nullptr, nullptr, &tcpSocket, &(Client::stopNow));
		Concurrency::Thread t(ReceiveThread_Client, data);
		while(!Client::stopNow)
		{
			msSleep(TICK_WAITTIME_TCP);
			if(timer.GetElapsedTime()>2000)
			{
				Append(Command::Heartbeat);
				timer.Reset();
			}
			if(DataSize()) Send();
		}
	}
	void TcpClient::AutoSendLoop()
	{
		while(!AutoSender::stopNow)
		{
			Packet tmp;
			
			autoSendMutex.Lock();
			for(auto it=objectsToSend.begin(); it!=objectsToSend.end(); ++it)
			{
				switch((Command)it->first)
				{
					case Command::String:
						for(auto values=it->second.begin(); values!=it->second.end(); ++values)
							Network::Append(Command::String, (*(std::string*)(*values)), tmp);
						break;
				}
			}
			autoSendMutex.Unlock();
			while(!IsSent() && !AutoSender::stopNow) {msSleep(TICK_WAITTIME_TCP/2);}
			Concurrency::Lock l(canAppend); 
			while(!tmp.Size()){uchar data; tmp>>data; packet<<data;}
		}
	}

	void UdpClient::Connect(const char* addr, ushort port)
	{
		serverAddress = IpAddress(addr);
		serverPort = port;
		udpSocket.SetBlocking(true);
		udpSocket.Bind();
		Start();
	}
	void UdpClient::ClientLoop()
	{
		Packet p;
		while(!stopNow)
		{
			UdpSend(Command::String, std::string("UDP Data."), &udpSocket, serverAddress, serverPort, p);
			msSleep(100);
		}
	}
}
