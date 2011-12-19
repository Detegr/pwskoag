#include "Network.h"
#include "Network_common.h"
#include <iostream>
#include <sys/time.h>

namespace pwskoag
{
	void ReceiveThread(void *args)
	{
		ThreadData* data=(ThreadData*)args;
		Mutex* lock=data->lock;
		C_Timer* timer=data->timer;
		TcpSocket* client=data->socket;
		bool* stopNow=data->stopNow;
		delete data;

		fd_set set;
		int recv=0;
		struct timeval tv;
		while(!*stopNow)
		{
			tv.tv_sec=4;
			tv.tv_usec=0;
			Packet p;
			FD_ZERO(&set);
			FD_SET(client->fd, &set);
			recv=select(client->fd+1, &set, NULL, NULL, &tv);
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
							case String:
								{
									std::string str;
									p>>str;
									std::cout << "Str: " << str << std::endl;
									break;
								}
							case Heartbeat:
								lock->Lock();
								timer->M_Reset();
								lock->Unlock();
								std::cout << "Beat from " << client->GetIp() << ":" << client->GetPort() << std::endl;
								break;
							case Disconnect:		
								lock->Lock();
								client->Clear();
								lock->Unlock();
								std::cout << "Client disconnected." << std::endl;
								pthread_exit(0);
								break;
							case EOP: goto EndOfPacket;
							default: break;
						}
					}
					EndOfPacket:
					Packet toClient;
					std::string str("Hi, this is server speaking.");
					if(!TcpSend(String, str, client, toClient))
					{
						std::cout << "Client disconnected: Terminating the connection." << std::endl;
						pthread_exit(0);
					}
					std::cout << "Sent: " << str << std::endl;
				}
				else
				{
					lock->Lock();
					client->Clear();
					lock->Unlock();
					std::cout << "Client disconnected." << std::endl;
					pthread_exit(0);
				}
			}
			else
			{
				lock->Lock();
				client->Clear();
				lock->Unlock();
				std::cout << "Client timed out" << std::endl;
				pthread_exit(0);
			}
		}
	}

	void TcpServer::ServerLoop()
	{
		tcpListener.Bind();
		tcpListener.Listen();
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
						if(header==Connect)
						{
							std::cout << "Client connected" << std::endl;
							clients.push_back(std::make_pair((Thread *)NULL, LocalThreadData(client)));
							ThreadData* data=new ThreadData(&clients.back().second.lock, &clients.back().second.timer, client, &stopNow);
							Thread* run=new Thread(ReceiveThread, (void*)data);
							clients.back().first=run;
						}
					}
				}
			}
			
			for(t_Clients::iterator it=clients.begin(); it!=clients.end(); ++it)
			{
				it->second.lock.Lock();
				bool closed=it->second.socket->Closed();
				it->second.lock.Unlock();
				if(closed)
				{
					std::cout << "Removed disconnected client from clients." << std::endl;
					it->first->Join();
					it->second.lock.Lock();
					delete it->first;
					delete it->second.socket;
					it->second.lock.Unlock();
					it=clients.erase(it);
					continue;
				}
			}
		}
		tcpListener.Close();

		if(clients.size()>0) {std::cout << "There were " << clients.size() << " clients connected." << std::endl;}
		std::cout << "Shut down successful." << std::endl;
	}
	void ReceiveThread_UDP(void *args)
	{
	}
	void UdpServer::ServerLoop()
	{
		Packet p;
		while(!stopNow)
		{
			t_Clients clients = master->GetClients();
			for(t_Clients::iterator it=clients.begin(); it!=clients.end(); ++it)
			{
				p.Clear();
				IpAddress ip = it->second.socket->GetIp();
				ushort port = it->second.socket->GetPort();
				if(udpSocket.Receive(p, ip, port))
				{
					std::cout << "Got data from: " << it->second.socket->GetIp() << std::endl;
				}
			}
			msSleep(TICK_WAITTIME_UDP);
		}
	}
		
	TcpServer::~TcpServer()
	{
		Lock lock(selfMutex);
		for(t_Clients::iterator it=clients.begin(); it!=clients.end(); ++it) delete it->first;
	}
	void TcpClient::M_Connect(const char* addr, ushort port)
	{
		Lock lock(Client::selfMutex);
		serverAddress=addr;
		serverPort=port;
		tcpSocket=TcpSocket(IpAddress(addr), port);
		tcpSocket.Connect();
		Client::Start();
		Send(Connect);
	}
	void TcpClient::M_Disconnect()
	{
		Send(Disconnect);
		Client::Stop();
		Lock lock(Client::selfMutex);
		tcpSocket.Disconnect();
	}

	void ReceiveThread_Client(void *args)
	{
		ThreadData* data=(ThreadData*)args;
		TcpSocket* tcpSocket=data->socket;
		bool* stopNow=data->stopNow;
		delete data;
		fd_set set;
		struct timeval tv;
		tv.tv_sec=1;
		tv.tv_usec=0;
		while(!*stopNow)
		{
			Packet p;
			uchar header=0;
			bool end=false;
			FD_SET(tcpSocket->fd, &set);
			tv.tv_sec=1;
			tv.tv_usec=0;
			int recv=select(tcpSocket->fd+1, &set, NULL, NULL, &tv);
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
							case String:
							{
								std::string str;
								p >> str;
								std::cout << str << std::endl;
								break;
							}
							case EOP:
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
		pthread_exit(0);
	}

	void TcpClient::ClientLoop()
	{
		C_Timer timer;
		ThreadData* data=new ThreadData(NULL, NULL, &tcpSocket, &(Client::stopNow));
		Thread t(ReceiveThread_Client, data);
		while(!Client::stopNow)
		{
			msSleep(TICK_WAITTIME_TCP);
			Append(String, std::string(":))"));
			if(timer.M_Get()>2000)
			{
				Append(Heartbeat);
				timer.M_Reset();
			}
			if(DataSize()) Send();
		}
	}
	void UdpClient::M_Connect(const char* addr, ushort port)
	{
		serverAddress = IpAddress(addr);
		serverPort = port;
		udpSocket=UdpSocket(serverAddress, port);
		udpSocket.Bind();
		Start();
	}
	void UdpClient::ClientLoop()
	{
		Packet p;
		while(!stopNow)
		{
			UdpSend(String, std::string("UDP Data."), &udpSocket, serverAddress, serverPort, p);
			msSleep(100);
		}
	}
}
