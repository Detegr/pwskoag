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
			selector.Wait(TICK_WAITTIME_TCP);
			if(selector.IsReady(tcpListener))
			{
				sf::TcpSocket* client = new sf::TcpSocket;
				if(tcpListener.Accept(*client) == sf::Socket::Done)
				{
					sf::Packet p;
					if(client->Receive(p)==sf::Socket::Done)
					{
						uchar header; p>>header;
						if(header==Command::Connect)
						{
							selector.Add(*client);
							std::cout << "Client connected" << std::endl;
							clients.push_back(std::make_pair(client, sf::Clock()));
						}
					}
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
							uchar header=0;
							for(;;)
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
										std::cout << "Beat from " << client->GetRemoteAddress() << ":" << client->GetRemotePort() << std::endl;
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
								sf::Packet toClient;
								std::string str("Hi, this is server speaking.");
								TcpSend(Command::String, str, client, toClient);
								std::cout << "Sent: " << str << std::endl;
							}
						}
					}
				}
			}
		}
		tcpListener.Close();

		if(clients.size()>0) {std::cout << "There were " << clients.size() << " clients connected." << std::endl;}
		std::cout << "Shut down successful." << std::endl;
	}
	void UdpServer::ServerLoop()
	{
		sf::Packet p;
		while(!stopNow)
		{
			auto clients = master->GetClients();
			for(auto it=clients.begin(); it!=clients.end(); ++it)
			{
				p.Clear();
				sf::IpAddress ip = it->first->GetRemoteAddress();
				ushort port = it->first->GetRemotePort();
				if(udpSocket.Receive(p, ip, port)==sf::Socket::Done)
				{
					std::cout << "Got data from: " << it->first->GetRemoteAddress() << std::endl;
				}
			}
			msSleep(TICK_WAITTIME_UDP);
		}
	}
		
	TcpServer::~TcpServer()
	{
		sf::Lock lock(selfMutex);
		for(auto it=clients.begin(); it!=clients.end(); ++it) delete it->first;
	}
	void TcpClient::Connect(const char* addr, ushort port)
	{
		sf::Lock lock(Client::selfMutex);
		serverAddress=addr;
		serverPort=port;
		tcpSocket.SetBlocking(false);
		/*
		if(tcpSocket.Connect(serverAddress, serverPort)!=sf::Socket::Done)
		{
			std::cerr << "Couldn't connect to " << addr << ":" << port << std::endl;
			return;
		}*/
		tcpSocket.Connect(serverAddress, serverPort);
		Send(Command::Connect);
		Client::Start();
		AutoSender::Start();
	}
	void TcpClient::Disconnect()
	{
		Client::Stop();
		AutoSender::Stop();
		sf::Lock lock(Client::selfMutex);
		Send(Command::Disconnect);
		tcpSocket.Disconnect();
	}
	void TcpClient::ClientLoop()
	{
		sf::Clock timer;
		Send(Command::Heartbeat);
		sf::Packet p;
		while(!Client::stopNow)
		{
			p.Clear();
			uchar header=0;
			if(tcpSocket.Receive(p)==sf::Socket::Done)
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
							std::cout << "Invalid packet, skipping..." << std::endl; break;
					}
				}
			}
			EndOfPacket:
			msSleep(TICK_WAITTIME_TCP);
			if(timer.GetElapsedTime()>2000)
			{
				Append(Command::Heartbeat);
				timer.Reset();
			}
			Send();
		}
	}
	void TcpClient::AutoSendLoop()
	{
		while(!AutoSender::stopNow)
		{
			sf::Packet tmp;
			
			autoSendMutex.Lock();
			for(auto it=objectsToSend.begin(); it!=objectsToSend.end(); ++it)
			{
				switch((Command)it->first)
				{
					case Command::String:
						tmp << *(std::string*)(it->second);
						break;
				}
			}
			autoSendMutex.Unlock();
			while(!IsSent() && !AutoSender::stopNow) {msSleep(TICK_WAITTIME_TCP/2);}
			Append(Command::String, tmp);
		}
	}

	void UdpClient::Connect(const char* addr, ushort port)
	{
		serverAddress = sf::IpAddress(addr);
		serverPort = port;
		udpSocket.SetBlocking(false);
		udpSocket.Bind(sf::Socket::AnyPort);
		Start();
	}
	void UdpClient::ClientLoop()
	{
		sf::Packet p;
		while(!stopNow)
		{
			UdpSend(Command::String, std::string("UDP Data."), &udpSocket, serverAddress, serverPort, p);
			msSleep(100);
		}
	}
}
