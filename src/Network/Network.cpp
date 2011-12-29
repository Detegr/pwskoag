#include <Util/Version.h>
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

	void UdpClient::Send(IpAddress& ip, ushort port)
	{
		Append(EOP);
		Lock l(m_Lock);
		udpSocket.Send(packet, ip, port);
		packet.Clear();
	}

	void TcpClient::Send()
	{
		Append(EOP);
		Lock l(m_Lock);
		tcpSocket.Send(packet);
		packet.Clear();
	}

	void TCPReceiveThread_Server(void *args)
	{
		ThreadData* data=(ThreadData*)args;
		Mutex* lock=data->lock;
		C_Timer* timer=data->timer;
		TcpSocket* client=(TcpSocket*)data->socket;
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
								std::cout << "Str: " << str << " from " << client->GetIp() << ":" << client->GetPort() << std::endl;
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
								bool ok=false;
								do
								{
									ok=true;
									ushort id=rand()%(((ushort)1)<<15);
									for(t_Clients::iterator it=clients.begin(); it!=clients.end(); ++it)
									{
										if(it->second.socket->M_Id()==id) {ok=false; break;}
									}
									client->M_Id(id);
								} while(!ok);
								p << client->M_Id();
								std::cout << "Generated id: " << client->M_Id() << std::endl;
								std::cout << "Client connected" << std::endl;
								clients.push_back(std::make_pair((Thread *)NULL, LocalThreadData(client)));
								ThreadData* data=new ThreadData(&clients.back().second.lock, &clients.back().second.timer, client, &stopNow);
								Thread* run=new Thread(TCPReceiveThread_Server, (void*)data);
								clients.back().first=run;
								client->Send(p);
							}
						}
					}
				}
			}
			for(t_Clients::iterator it=clients.begin(); it!=clients.end(); ++it)
			{
				it->second.lock.Lock();
				bool closed=it->second.socket->M_Closed();
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
	void UDPReceiveThread_Client(void *args)
	{
		Packet p;
		ThreadData* data=(ThreadData*)args;
		Mutex* lock=data->lock;
		C_Timer* timer=data->timer;
		UdpSocket* client=(UdpSocket*)data->socket;
		bool* stopNow=data->stopNow;
		delete data;

		while(!*stopNow)
		{
			p.Clear();
			if(client->Receive(p))
			{
				uchar header=0;
				bool eop=false;
				while(!eop)
				{
					p>>header;
					switch (header)
					{
						case String:
						{
							std::string str;
							p>>str;
							std::cout << "UDP from server: " << str << std::endl;
							break;
						}
						case EOP:
						{
							eop=true;
							break;
						}
					}
				}

			}
			msSleep(TICK_WAITTIME_UDP);
		}
	}

	void UdpServer::ServerLoop()
	{
		const t_Clients& c = master->GetClients();
		Packet p;
		Selector sel;
		while(!stopNow)
		{
			sel.Clear();
			sel.Add(udpSocket);
			if(sel.Wait(TICK_WAITTIME_UDP))
			{
				if(sel.IsReady(udpSocket))
				{
					p.Clear();
					IpAddress ip;
					ushort port;
					if(udpSocket.Receive(p, &ip, &port))
					{
						for(t_Clients::const_iterator it=c.begin(); it!=c.end(); ++it)
						{
							TcpSocket* s=dynamic_cast<TcpSocket*>(it->second.socket);
							if(ip==s->GetIp())
							{
								if(!s->M_UdpPort())
								{
									for(;;)
									{
										uchar header=0;
										p >> header;
										if(header==Connect)
										{
											ushort id;
											p>>id;
											if(id==s->M_Id())
											{
												s->M_UdpPort(port);
												std::cout << "Bound id " << id << " to UDP port " << port << std::endl;
												p.Clear();
												p<<HandShake<<EOP;
												if(sel.WaitWrite(TICK_WAITTIME_UDP))
												{
													if(sel.IsReady(udpSocket))
													{
														udpSocket.Send(p, ip, port);
														std::cout << "Sent confirmation to " << ip  << ":" << port << std::endl;
													}
													else std::cout << "Client wasn't ready." << std::endl;
												}
												else std::cout << "Socket wasn't ready to write." << std::endl;
											}
											else std::cout << "Wrong ID, discarding connection..." << std::endl;
										}
										break;
									}
									continue;
								}
								if(s->M_UdpPort()==port)
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
												std::cout << "Str: " << str << " from " << ip << ":" << port << std::endl;
												break;
											}
											case EOP:
											{
												Packet p;
												p << String << std::string("UDP response") << EOP;
												udpSocket.Send(p, ip, port);
												break;
											}
										}
									}
									break;
								}
							}
						}
					}
					else std::cerr << "Data wasn't ready." << std::endl;
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
					ushort id;
					p >> id;
					tcpSocket.M_Id(id);
					std::cout << "Got handshake with id: " << tcpSocket.M_Id() << std::endl;
					Start();
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

	void TCPReceiveThread_Client(void *args)
	{
		ThreadData* data=(ThreadData*)args;
		TcpSocket* tcpSocket=(TcpSocket*)data->socket;
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
		Thread t(TCPReceiveThread_Client, data);
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
		m_Address=IpAddress(addr);
		m_Port=port;
		udpSocket=UdpSocket(m_Address, m_Port);

		Selector s;
		s.Add(udpSocket);
		if(s.WaitWrite(CONNECTTIME))
		{
			if(s.IsReady(udpSocket))
			{
				std::cout << "Connecting to UDP server..." << std::endl;
				packet << Connect << m_Master->M_Id();
				udpSocket.Send(packet, m_Address, m_Port);
			}
		}
		Packet p;
		if(s.Wait(CONNECTTIME))
		{
			if(s.IsReady(udpSocket))
			{
				if(udpSocket.Receive(p))
				{
					uchar c; p>>c;
					if(c==HandShake)
					{
						std::cout << "Got UDP confirmation." << std::endl;
						Start();
						return true;
					}
				}
				else {std::cout << "Received broken data when connecting to UDP server." << std::endl; return false;}
			}
			else {std::cout << "Server didn't send confirmation data." << std::endl; return false;}
		}
		else {std::cout << "Couldn't connect to UDP server, connection timed out." << std::endl; return false;}
		return true;
	}
	void UdpClient::ClientLoop()
	{
		ThreadData* data=new ThreadData(NULL, NULL, &udpSocket, &(Client::stopNow));
		Thread t(UDPReceiveThread_Client, data);
		while(!stopNow)
		{
			Append(String, std::string("UDP Data."));
			Send(m_Master->serverAddress, m_Port);
			msSleep(100);
		}
	}
}
