#include "Version.h"
#include "Network.h"
#include "Network_common.h"
#include <iostream>
#include <sys/time.h>
#include <assert.h>

namespace pwskoag
{
	static void EndOfPacket(TcpSocket* client)
	{
		Packet toClient;
		std::string str("Hi, this is server speaking.");
		if(!TcpSend(String, str, client, toClient))
		{
			std::cout << "Client disconnected: Terminating the connection." << std::endl;
		}
		std::cout << "Sent: " << str << std::endl;
	}
	void ReceiveThread(void *args)
	{
		ThreadData* data=(ThreadData*)args;
		Mutex* lock=data->lock;
		C_Timer* timer=data->timer;
		TcpSocket* client=data->socket;
		bool* stopNow=data->stopNow;
		delete data;

		struct timeval tv;
		Selector s;
		while(!*stopNow)
		{
			Packet p;
			s.Clear();
			s.Add(*client);
			s.Wait(4000);
			if(s.IsReady(*client))
			{
				if(client->Receive(p))
				{
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
								return;
							case EOP: EndOfPacket(client); break;
							default: break;
						}
					}
				}
				else
				{
					lock->Lock();
					client->Clear();
					lock->Unlock();
					std::cout << "Client disconnected." << std::endl;
				}
			}
			else
			{
				lock->Lock();
				client->Clear();
				lock->Unlock();
				std::cout << "Client timed out" << std::endl;
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
			selector.Wait(TICK_WAITTIME_TCP);
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
							uint version; p>>version;
							std::cout << "Version: " << version << std::endl;
							if(!C_Version::M_Equal(version))
							{
								std::cerr << "Version mismatch!" << std::endl;
								delete client;
							}
							else
							{
								p.Clear();
								p<<HandShake;
								client->Send(p);
								std::cout << "Client connected" << std::endl;
								clients.push_back(std::make_pair((Thread *)NULL, LocalThreadData(client)));
								ThreadData* data=new ThreadData(&clients.back().second.lock, &clients.back().second.timer, client, &stopNow);
								Thread* run=new Thread(ReceiveThread, (void*)data);
								clients.back().first=run;
							}
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
		Selector s;
		while(!stopNow)
		{
			s.Clear();
			t_Clients clients = master->GetClients();
			for(t_Clients::iterator it=clients.begin(); it!=clients.end(); ++it) s.Add(udpSocket);
			if(s.Wait(TICK_WAITTIME_UDP))
			{
				for(t_Clients::iterator it=clients.begin(); it!=clients.end(); ++it)
				{
					if(s.IsReady(udpSocket))
					{
						p.Clear();
						IpAddress ip = it->second.socket->GetIp();
						ushort port = it->second.socket->GetPort();
						if(udpSocket.Receive(p))
						{
							while(p.Size())
							{
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
										case EOP: break;
										default: break;
									}
								}
							}
						}
					}
				}
			}
		}
	}
		
	TcpServer::~TcpServer()
	{
		Lock lock(selfMutex);
		for(t_Clients::iterator it=clients.begin(); it!=clients.end(); ++it) delete it->first;
	}
	bool TcpClient::M_Connect(const char* addr, ushort port)
	{
		Lock lock(Client::selfMutex);
		serverAddress=addr;
		serverPort=port;
		tcpSocket=TcpSocket(IpAddress(addr), port);
		tcpSocket.Connect();
		packet << Connect << C_Version::M_Get();
		Send();
		Packet p;
		Selector s;
		s.Add(tcpSocket);
		s.Wait(CONNECTTIME);
		if(s.IsReady(tcpSocket))
		{
			if(tcpSocket.Receive(p))
			{
				uchar c; p>>c;
				if(c==HandShake)
				{
					std::cout << "Got handshake. Launching threads..." << std::endl;
					Client::Start();
					return true;
				}
			}
			else {std::cout << "Couldn't connect to server." << std::endl; return false;}
		}
		else {std::cout << "Couldn't connect to server." << std::endl; return false;}
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
		bool connect=true;
		delete data;
		struct timeval tv;
		Selector s;
		while(!*stopNow)
		{
			Packet p;
			uchar header=0;
			bool end=false;
			s.Clear();
			s.Add(*tcpSocket);
			s.Wait(1000);
			if(s.IsReady(*tcpSocket))
			{
				if(tcpSocket->Receive(p))
				{
					for(;;)
					{
						p>>header;
						std::cout << "GOT HEADER: " << (int)header << std::endl;
						switch (header)
						{
							case String:
							{
								std::string str;
								p >> str;
								std::cout << "GOT: " << str << std::endl;
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
			Send();
		}
	}
	bool UdpClient::M_Connect(const char* addr, ushort port)
	{
		serverAddress = IpAddress(addr);
		serverPort = port;
		udpSocket=UdpSocket(serverAddress, port);
		Start();
		return true;
	}
	void UdpClient::ClientLoop()
	{
		Packet p;
		while(!stopNow)
		{
			p.Clear();
			std::cout << "Sending udp data to " << serverAddress << ":" << serverPort << std::endl;
			Append(String, std::string("UDP Data."));
			Send();
			msSleep(100);
		}
	}
}
