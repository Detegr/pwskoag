#include <Util/Version.h>
#include "Client.h"
#include <Network/ThreadData.h>
#include <Network/Netcommand.h>
#include <Util/DataUtils.h>

namespace pwskoag
{
	PWSKOAG_API void C_Client::M_Start()
	{
		C_Lock lock(m_SelfMutex);
		if(!m_SelfThread)
		{
			m_SelfThread = new C_Thread(C_Client::ClientInitializer, this);
		}
		else std::cerr << "Client already running!" << std::endl;
	}
	PWSKOAG_API void C_Client::M_Stop()
	{
		C_Lock lock(m_SelfMutex);
		m_StopNow=true;
		if(m_SelfThread) {std::cout << "Disconnecting...";std::cout.flush(); delete m_SelfThread; m_SelfThread=NULL;std::cout<<"DONE!"<<std::endl;}
		else std::cerr << "Client already stopped!" << std::endl;
	}
	PWSKOAG_API void C_Client::M_ForceStop()
	{
		//Lock lock(m_SelfMutex);
		//if(m_SelfThread) {m_SelfThread->Terminate(); delete m_SelfThread; m_SelfThread=NULL;}
	}
	void C_Client::ClientInitializer(void* args)
	{
		C_Client* s = (C_Client*)args;
		s->ClientLoop();
	}
	PWSKOAG_API C_Client::~C_Client()
	{
		if(m_SelfThread) std::cerr << "!! Client not stopped and still shutting down !!" << std::endl;
	}
	
	void C_TcpClient::M_Send()
	{
		m_Packet << (uchar)C_Netcommand::EOP;
		C_Lock l(m_Lock);
		m_TcpSocket.M_Send(m_Packet);
		m_Packet.M_Clear();
	}

	void C_UdpClient::M_Send(const C_IpAddress& ip, ushort port)
	{
		m_Packet << (uchar)C_Netcommand::EOP;
		C_Lock l(m_Lock);
		m_UdpSocket.M_Send(m_Packet, ip, port);
		m_Packet.M_Clear();
	}

	static void UDPReceive(void *args)
	{
		C_Packet p;
		C_ThreadData* data=(C_ThreadData*)args;
		C_UdpSocket* client=(C_UdpSocket*)data->m_Socket;
		t_Entities* plrs=data->m_Players;
		bool* m_StopNow=data->m_StopNow;
		
		while(!*m_StopNow)
		{
			p.M_Clear();
			if(client->M_Receive(p))
			{
				uchar header=0;
				bool eop=false;
				while(p.M_Size() && !eop)
				{
					p>>header;
					switch (header)
					{
						case C_Netcommand::Update:
						{
							ushort id;
							C_Vec2 pos;
							p>>id; p>>pos;
							C_Lock(*data->m_PlayerLock);
							C_Entity* e = g_EntityById(*plrs, id);
							if(e)
							{
								C_ClientPlayer* plr=dynamic_cast<C_ClientPlayer *>(e);
								plr->M_Position(pos);
							}
							break;
						}
						case C_Netcommand::EOP:
						{
							eop=true;
							break;
						}
					}
				}
				
			}
			g_Sleep(TICK_WAITTIME_UDP);
		}
	}
	
	static void M_CheckNewPlayers(ushort id, t_Entities& plrs, C_Mutex& playerlock)
	{
		C_Lock l(playerlock);
		bool newplr=true;
		for(t_Entities::iterator it=plrs.begin(); it!=plrs.end(); ++it)
		{
			C_ClientPlayer* plr=dynamic_cast<C_ClientPlayer*>(*it);
			if(plr->M_Id()==id) {newplr=false; break;}
		}
		if(newplr)
		{
			std::cout << "New player: " << id << std::endl;
			plrs.push_back(new C_ClientPlayer);
			plrs.back()->M_Id(id);
		}
	}
	
	PWSKOAG_API bool C_TcpClient::M_Connect(const char* addr, ushort port)
	{
		C_Lock lock(C_Client::m_SelfMutex);
		m_ServerAddress=addr;
		m_ServerPort=port;
		m_TcpSocket=C_TcpSocket(C_IpAddress(addr), port);
		m_TcpSocket.M_Connect();
		m_Packet << C_Netcommand::Connect << C_Version::M_Get();
		M_Send();
		C_Packet p;
		C_Selector s;
		s.M_Add(m_TcpSocket);
		s.M_Wait(CONNECTTIME);
		if(s.M_IsReady(m_TcpSocket))
		{
			if(m_TcpSocket.M_Receive(p))
			{
				uchar c; p>>c;
				if(c==C_Netcommand::Connect)
				{
					ushort id;
					p >> id;
					m_TcpSocket.M_Id(id);
					std::cout << "Got handshake with id: " << m_TcpSocket.M_Id() << std::endl;
					C_ClientPlayer* n = new C_ClientPlayer(&m_TcpSocket, &m_Packet);
					m_OwnPlayer=n;
					m_Players.push_back(dynamic_cast<C_Entity*>(m_OwnPlayer));
					m_Players.back()->M_Id(id);
					while(p.M_Size())
					{
						p>>c;
						if(c==C_Netcommand::ConnectedPlayers)
						{
							ushort id; p>>id;
							M_CheckNewPlayers(id, m_Players, m_PlayerLock);
							/*
							for(t_Entities::iterator it=m_Players.begin(); it!=m_Players.end(); ++it)
							{
								C_ClientPlayer* plr=static_cast<C_ClientPlayer*>(*it);
								if(plr->M_Id()==id) plr->M_SetStr(str);
							}
							*/
						}
					}
					M_Start();
					return true;
				}
			}
			else {std::cout << "Couldn't connect to server." << std::endl; return false;}
		}
		std::cout << "Couldn't connect to server." << std::endl;
		return false;
	}
	
	PWSKOAG_API void C_TcpClient::M_Disconnect()
	{
		m_Packet<<(uchar)C_Netcommand::Disconnect;
		M_Send();
		M_Stop();
		//Lock lock(Client::m_SelfMutex);
		m_TcpSocket.M_Disconnect();
		for(t_Entities::iterator it=m_Players.begin(); it!=m_Players.end(); ++it)
		{
			delete *it;
		}
	}
	
	static void TCPReceive(void *args)
	{
		C_ThreadData* data=(C_ThreadData*)args;
		C_TcpSocket* tcpSocket=(C_TcpSocket*)data->m_Socket;
		t_Entities* plrs=data->m_Players;
		bool* m_StopNow=data->m_StopNow;
		C_Mutex* playerlock=data->m_PlayerLock;
		C_Selector s;
		while(!*m_StopNow)
		{
			C_Packet p;
			uchar header=0;
			bool end=false;
			s.M_Clear();
			s.M_Add(*tcpSocket);
			s.M_Wait(1000);
			if(s.M_IsReady(*tcpSocket))
			{
				if(tcpSocket->M_Receive(p))
				{
					while(p.M_Size() && !end)
					{
						p>>header;
						switch (header)
						{
							case C_Netcommand::HeartBeat: // Change this
							{
								ushort id;
								std::string str;
								p>>id;
								M_CheckNewPlayers(id, *plrs, *playerlock);
								std::cout << "new player: " << id << std::endl;
								/*
								playerlock->M_Lock();
								for(t_Entities::iterator it=plrs->begin(); it!=plrs->end(); ++it)
								{
									C_ClientPlayer* plr=static_cast<C_ClientPlayer*>(*it);
									if(plr->M_Id()==id) plr->M_SetStr(str);
								}
								playerlock->M_Unlock();
								std::cout << "GOT: " << id << ", " << str << ", size: " << p.M_Size() << std::endl;
								*/
								break;
							}
							case C_Netcommand::Disconnect:
							{
								ushort id;
								p>>id;
								std::cout << "Client " << id << " disconnected." << std::endl;
								playerlock->M_Lock();
								for(t_Entities::iterator it=plrs->begin(); it!=plrs->end(); ++it)
								{
									C_ClientPlayer* plr=dynamic_cast<C_ClientPlayer*>(*it);
									if(plr->M_Id()==id)
									{
										delete *it;
										plrs->erase(it);
										break;
									}
								}
								playerlock->M_Unlock();	
								data->m_PlayersModified->M_Signal();							
								break;
							}
							case C_Netcommand::EOP:
								std::cout << "EOP" << std::endl;
								//end=true;
								break;
							default:
								std::cout << "Invalid packet. Terminating." << std::endl;
								std::cout << "Packet size: " << p.M_Size() << std::endl;
								std::cout << "Header: " << header << std::endl;
								break;
						}
					}
				}
			}
		}
	}
	
	PWSKOAG_API void C_TcpClient::ClientLoop()
	{
		C_Timer timer;
		C_ThreadData data(C_ThreadData(NULL, &m_TcpSocket, NULL, &m_Players, &m_PlayerLock, &(C_Client::m_StopNow)));
		C_Thread t(TCPReceive, &data);
		while(!m_StopNow)
		{
			g_Sleep(TICK_WAITTIME_TCP);
			if(timer.M_Get()>2000)
			{
				m_Packet<<(uchar)C_Netcommand::HeartBeat;
				timer.M_Reset();
			}
			if(m_Packet.M_Size()) M_Send();
		}
	}
	PWSKOAG_API bool C_UdpClient::M_Connect(const char* addr, ushort port)
	{
		if(m_Initialized)
		{
			C_IpAddress m_Address(addr);
			m_Port=port;
			m_UdpSocket=C_UdpSocket(m_Address, m_Port);
			
			C_Selector s;
			s.M_Add(m_UdpSocket);
			if(s.M_WaitWrite(CONNECTTIME))
			{
				if(s.M_IsReady(m_UdpSocket))
				{
					std::cout << "Connecting to UDP server..." << std::endl;
					m_Packet << (uchar)C_Netcommand::Connect << m_Master->M_Id();
					m_UdpSocket.Send(m_Packet, m_Address, m_Port);
				}
			}
			C_Packet p;
			if(s.M_Wait(CONNECTTIME))
			{
				if(s.M_IsReady(udpSocket))
				{
					if(C_UdpSocket.M_Receive(p))
					{
						uchar c; p>>c;
						if(c==C_Netcommand::Connect)
						{
							std::cout << "Got UDP confirmation." << std::endl;
							M_Start();
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
		else {std::cout << "Socket not initialized." << std::endl; return false;}
	}

	PWSKOAG_API void C_UdpClient::ClientLoop()
	{
		C_ThreadData data(NULL, &m_UdpSocket, NULL, &m_Master->M_Players(), &m_Master->m_PlayerLock, NULL, &(Client::m_StopNow));
		C_Thread t(UDPReceive, &data);
		while(!m_StopNow)
		{
			//packet<<MessageTimer<<m_Master->M_OwnPlayer()->M_Id();
			//Send(m_Master->serverAddress, m_Port);
			g_Sleep(100);
		}
	}
}
