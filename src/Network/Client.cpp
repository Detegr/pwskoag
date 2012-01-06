#include "Client.h"

namespace pwskoag
{
	PWSKOAG_API void Client::Start()
	{
		C_Lock lock(selfMutex);
		if(!selfThread)
		{
			selfThread = new C_Thread(Client::ClientInitializer, this);
		}
		else std::cerr << "Client already running!" << std::endl;
	}
	PWSKOAG_API void Client::Stop()
	{
		C_Lock lock(selfMutex);
		stopNow=true;
		if(selfThread) {std::cout << "Disconnecting...";std::cout.flush(); delete selfThread; selfThread=NULL;std::cout<<"DONE!"<<std::endl;}
		else std::cerr << "Client already stopped!" << std::endl;
	}
	PWSKOAG_API void Client::ForceStop()
	{
		//Lock lock(selfMutex);
		//if(selfThread) {selfThread->Terminate(); delete selfThread; selfThread=NULL;}
	}
	void Client::ClientInitializer(void* args)
	{
		Client* s = (Client*)args;
		s->ClientLoop();
	}
	PWSKOAG_API Client::~Client()
	{
		if(selfThread) std::cerr << "!! Client not stopped and still shutting down !!" << std::endl;
	}
	
	void TcpClient::Send()
	{
		Append(EOP);
		C_Lock l(m_Lock);
		tcpSocket.Send(packet);
		packet.M_Clear();
	}

	void UdpClient::Send(IpAddress& ip, ushort port)
	{
		Append(EOP);
		C_Lock l(m_Lock);
		udpSocket.Send(packet, ip, port);
		packet.M_Clear();
	}

	void UDPReceive(void *args)
	{
		C_Packet p;
		C_ThreadData* data=(C_ThreadData*)args;
		C_Mutex* lock=data->lock;
		C_Timer* timer=data->timer;
		UdpSocket* client=(UdpSocket*)data->socket;
		bool* stopNow=data->stopNow;
		delete data;
		
		while(!*stopNow)
		{
			p.M_Clear();
			if(client->Receive(p))
			{
				uchar header=0;
				bool eop=false;
				while(p.M_Size() && !eop)
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
	
	PWSKOAG_API bool TcpClient::M_Connect(const char* addr, ushort port)
	{
		C_Lock lock(Client::selfMutex);
		serverAddress=addr;
		serverPort=port;
		tcpSocket=TcpSocket(IpAddress(addr), port);
		tcpSocket.Connect();
		packet << TCPConnect << C_Version::M_Get();
		Send();
		C_Packet p;
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
					C_ClientPlayer* n = new C_ClientPlayer(this);
					m_OwnPlayer=n;
					m_Players.push_back(m_OwnPlayer);
					m_Players.back()->M_SetId(id);
					Start();
					return true;
				}
			}
			else {std::cout << "Couldn't connect to server." << std::endl; return false;}
		}
		std::cout << "Couldn't connect to server." << std::endl;
		return false;
	}
	
	PWSKOAG_API void TcpClient::M_Disconnect()
	{
		Send(Disconnect);
		Stop();
		//Lock lock(Client::selfMutex);
		tcpSocket.Disconnect();
		for(std::vector<C_Player *>::iterator it=m_Players.begin(); it!=m_Players.end(); ++it)
		{
			delete *it;
		}
	}
	
	void TCPReceive(void *args)
	{
		C_ThreadData* data=(C_ThreadData*)args;
		TcpSocket* tcpSocket=(TcpSocket*)data->socket;
		std::vector<C_Player *>* plrs=data->m_Players;
		bool* stopNow=data->stopNow;
		bool connect=true;
		delete data;
		Selector s;
		while(!*stopNow)
		{
			C_Packet p;
			uchar header=0;
			bool end=false;
			s.Clear();
			s.Add(*tcpSocket);
			s.Wait(1000);
			if(s.IsReady(*tcpSocket))
			{
				if(tcpSocket->Receive(p))
				{
					while(p.M_Size() && !end)
					{
						p>>header;
						switch (header)
						{
							case Integer:
							{
								int i;
								std::string str;
								p>>i;
								bool newplr=true;
								for(std::vector<C_Player *>::iterator it=plrs->begin(); it!=plrs->end(); ++it)
								{
									C_ClientPlayer* plr=dynamic_cast<C_ClientPlayer*>(*it);
									if(plr->M_Id()==i) {newplr=false; break;}
								}
								if(newplr)
								{
									std::cout << "New player: " << i << std::endl;
									plrs->push_back(new C_ClientPlayer(false));
									plrs->back()->M_SetId(i);
								}
								p>>str;
								std::cout << str << " for " << i << std::endl;
								for(std::vector<C_Player *>::iterator it=plrs->begin(); it!=plrs->end(); ++it)
								{
									C_ClientPlayer* plr=dynamic_cast<C_ClientPlayer*>(*it);
									if(plr->M_Id()==i) plr->M_SetStr(str);
								}
								std::cout << "GOT: " << i << ", " << str << ", size: " << p.M_Size() << std::endl;
								break;
							}
							case String:
							{
								std::string str;
								p >> str;
								std::cout << "GOT: " << str << std::endl;
								break;
							}
							case EOP:
								std::cout << "EOP" << std::endl;
								//end=true;
								break;
							default:
								std::cout << "Invalid packet. Terminating." << std::endl; break;
						}
					}
				}
			}
		}
	}
	
	PWSKOAG_API void TcpClient::ClientLoop()
	{
		C_Timer timer;
		C_ThreadData* data=new C_ThreadData(NULL, &tcpSocket, NULL, &m_Players, &(Client::stopNow));
		C_Thread t(TCPReceive, data);
		while(!stopNow)
		{
			msSleep(TICK_WAITTIME_TCP);
			if(timer.M_Get()>2000)
			{
				Append(Heartbeat);
				timer.M_Reset();
			}
			if(packet.M_Size()) Send();
		}
	}
	PWSKOAG_API bool UdpClient::M_Connect(const char* addr, ushort port)
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
				packet << UDPConnect << m_Master->M_Id();
				udpSocket.Send(packet, m_Address, m_Port);
			}
		}
		C_Packet p;
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

	PWSKOAG_API void UdpClient::ClientLoop()
	{
		C_ThreadData* data=new C_ThreadData(NULL, &udpSocket, NULL, NULL, &(Client::stopNow));
		C_Thread t(UDPReceive, data);
		while(!stopNow)
		{
			Append(String, std::string("UDP Data."));
			Send(m_Master->serverAddress, m_Port);
			msSleep(100);
		}
	}
}