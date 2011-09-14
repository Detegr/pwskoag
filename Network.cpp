#include "Network.h"
#include <iostream>

namespace Network
{
	void ReceiveThread(void *socket)
	{
		TcpSocket* client=(TcpSocket*) socket;
		while(true)
		{
			Packet p;
			if(client->Receive(p))
			{
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
							//lastHeartBeat.Reset();
							std::cout << "Beat from " << client->GetIp() << ":" << client->GetPort() << std::endl;
							break;
						case Command::Disconnect:		
							//it=clients.erase(it);
							//selector.Remove(*client);
							delete client;
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
					//it=clients.erase(it);
					//selector.Remove(*client);
					delete client;
					client=NULL;
					std::cout << "Client disconnected: Terminated the connection." << std::endl;
					pthread_exit(0);
				}
				std::cout << "Sent: " << str << std::endl;
			}
		}
	}

	void TcpServer::ServerLoop()
	{
		tcpListener.Bind();
		tcpListener.Listen();
		tcpListener.SetBlocking(false);
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
							clients.push_back(std::make_pair(new Concurrency::Thread(ReceiveThread, (void*)client), sf::Clock()));
						}
					}
				}
			}
			/*
			for(auto it=clients.begin(); it!=clients.end(); ++it)
			{
				TcpSocket* client = it->first;
				sf::Clock& lastHeartBeat = it->second;
				if(lastHeartBeat.GetElapsedTime() > TIMEOUTMS)
				{
					TcpSocket* s=it->first;
					selector.Remove(*it->first);
					it=clients.erase(it);
					delete(s);
					std::cout << "Client timed out." << std::endl;
				}
				if(selector.IsReady(*client))
				{
					Packet p;
					if(client->Receive(p))
					{
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
									lastHeartBeat.Reset();
									std::cout << "Beat from " << client->GetIp() << ":" << client->GetPort() << std::endl;
									break;
								case Command::Disconnect:		
									it=clients.erase(it);
									selector.Remove(*client);
									delete client;
									client=NULL;
									std::cout << "Client disconnected." << std::endl;
									break;
								case Command::EOP: goto EndOfPacket;
								default: break;
							}
						}
						EndOfPacket:
						if(client)
						{
							Packet toClient;
							std::string str("Hi, this is server speaking.");
							if(!TcpSend(Command::String, str, client, toClient))
							{
								it=clients.erase(it);
								selector.Remove(*client);
								delete client;
								client=NULL;
								std::cout << "Client disconnected: Terminated the connection." << std::endl;
								continue;
							}
							std::cout << "Sent: " << str << std::endl;
						}
					}
				}
			}*/
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
		Client::Stop();
		//AutoSender::Stop();
		Concurrency::Lock lock(Client::selfMutex);
		Send(Command::Disconnect);
		tcpSocket.Disconnect();
	}

	void ReceiveThread_Client(void *args)
	{
		TcpSocket* tcpSocket=(TcpSocket*)args;
		while(true)
		{
			Packet p;
			uchar header=0;
			if(tcpSocket->Receive(p))
			{
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
							goto EndOfPacket; 
						default:
							std::cout << "Invalid packet. Terminating." << std::endl; break;
					}
				}
			}
			EndOfPacket:
			std::cout << "Eop" << std::endl;
		}
	}

	void TcpClient::ClientLoop()
	{
		sf::Clock timer;
		Concurrency::Thread t(ReceiveThread_Client, &tcpSocket);
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
