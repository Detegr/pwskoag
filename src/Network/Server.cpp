#include "Version.h"
#include "Server.h"

namespace pwskoag
{
	PWSKOAG_API void Server::Start()
	{
		C_Lock lock(selfMutex);
		if(!selfThread)
		{
			stopNow=false;
			selfThread = new C_Thread(Server::ServerInitializer, this);
		}
		else std::cerr << "Server already running!" << std::endl;
	}
	PWSKOAG_API void Server::Stop()
	{
		C_Lock lock(selfMutex);
		stopNow=true;
		if(selfThread) {selfThread->M_Join(); delete selfThread; selfThread=NULL;}
		else std::cerr << "Server already stopped!" << std::endl;
	}
	PWSKOAG_API void Server::ForceStop()
	{
		//Lock lock(selfMutex);
		//if(selfThread) {selfThread->Terminate(); delete selfThread; selfThread=NULL;}
	}
	void Server::ServerInitializer(void* args)
	{
		Server* s = (Server*)args;
		s->ServerLoop();
	}
	PWSKOAG_API Server::~Server()
	{
		if(selfThread) std::cerr << "!! Server not stopped and still shutting down !!" << std::endl;
	}
	
	TcpServer::~TcpServer()
	{
		C_Lock lock(selfMutex);
		for(t_Clients::iterator it=m_Clients.begin(); it!=m_Clients.end(); ++it) delete it->first;
	}
	
	static void M_UpdatePlayers(const std::vector<C_Player*>& plrs, C_Mutex& lock)
	{
		C_Lock l(lock);
		for(std::vector<C_Player*>::const_iterator it=plrs.begin(); it!=plrs.end(); ++it)
		{
			C_ServerPlayer* plr=dynamic_cast<C_ServerPlayer*>(*it);
			for(std::vector<C_Player*>::const_iterator it2=plrs.begin(); it2!=plrs.end(); ++it2)
			{
				C_ServerPlayer* plr2=dynamic_cast<C_ServerPlayer*>(*it2);
				*(plr->m_Packet)
				<<(uchar)Message
				<<plr2->M_Id()
				<<plr2->M_GetStr();
			}
			plr->M_Send();
			plr->m_Packet->M_Clear();			
		}
	}
	
	static bool M_ParsePacket(C_Packet& p, e_Command header, C_ThreadData* data)
	{
		C_Mutex* lock=data->lock;
		C_Mutex* plrlock=data->m_PlayerLock;
		C_Timer* timer=data->timer;
		std::vector<C_Player *>* plrs=data->m_Players;
		TcpSocket* client=(TcpSocket*)data->socket;
		
		if(header==String)
		{
		}
		else if(header==Message)
		{
			/*
			 * Message:
			 * [int|std::string]
			 */
			int i; p>>i;
			std::string str; p>>str;
			std::cout << "ID: " << i << ": " << str << std::endl;
			lock->M_Lock();
			plrlock->M_Lock();
			for(std::vector<C_Player*>::iterator it=plrs->begin(); it!=plrs->end(); ++it)
			{
				if((*it)->M_Id()==i)
				{
					(*it)->M_SetStr(str);
					break;
				}
			}
			plrlock->M_Unlock();
			M_UpdatePlayers(*plrs, *plrlock);
			lock->M_Unlock();		
		}
		else if(header==Heartbeat)
		{
			lock->M_Lock();
			std::cout << "Time from last beat: " << timer->M_Get() << std::endl;
			timer->M_Reset();
			std::cout << "Time now: " << timer->M_Get() << std::endl;
			lock->M_Unlock();
			std::cout << "Beat from " << client->GetIp() << ":" << client->GetPort() << std::endl;
		}
		else if(header==Disconnect)
		{
			lock->M_Lock();
			client->Clear();
			lock->M_Unlock();
			std::cout << "Client disconnected." << std::endl;
			return false;
		}
		else if(header==EOP) return true;
		else throw std::runtime_error("Invalid packet header.");
		return true;
	}
	
	static void TCPReceive(void* args)
	{
		C_ThreadData* data=(C_ThreadData*)args;
		C_Mutex* lock=data->lock;
		TcpSocket* client=(TcpSocket*)data->socket;
		bool* stopNow=data->stopNow;
		
		Selector s;
		C_Packet p;
		while(!*stopNow)
		{
			s.Clear();
			s.Add(*client);
			s.Wait(4000);
			if(s.IsReady(*client))
			{
				if(client->Receive(p))
				{
					uchar header=0;
					while(p.M_Size())
					{
						p>>header;
						if(!M_ParsePacket(p, (e_Command)header, data)) {delete data; return;}
					}
				}
				else
				{
					lock->M_Lock();
					client->Clear();
					lock->M_Unlock();
					std::cout << "Client disconnected?" << std::endl;
					break;
				}
			}
			else
			{
				lock->M_Lock();
				client->Clear();
				lock->M_Unlock();
				std::cout << "Client timed out" << std::endl;
				break;
			}
		}
		delete data;
	}
	
	void TcpServer::M_NewPlayer(TcpSocket* client)
	{
		bool ok=false;
		do
		{
			ok=true;
			ushort id=rand()%(((ushort)1)<<15);
			m_ClientLock.M_Lock();
			for(t_Clients::iterator it=m_Clients.begin(); it!=m_Clients.end(); ++it)
			{
				if(it->second.socket->M_Id()==id) {ok=false; break;}
			}
			m_ClientLock.M_Unlock();
			client->M_Id(id);
		} while(!ok);
		std::cout << "Generated id: " << client->M_Id() << std::endl;
		std::cout << "Client connected" << std::endl;
		C_ServerPlayer* newplayer=new C_ServerPlayer(client, new C_Packet);
		newplayer->M_SetId(client->M_Id());
		*newplayer->m_Packet<<HandShake<<client->M_Id();
		
		m_PlayerLock.M_Lock();
		m_Players.push_back(newplayer);
		m_PlayerLock.M_Unlock();
		
		std::cout << "Players: " << m_Players.size() << std::endl;
		std::pair<C_Thread*, LocalThreadData> localdata(std::make_pair((C_Thread *)NULL, LocalThreadData(client)));
		C_ThreadData* data=new C_ThreadData(&localdata.second.lock,client,localdata.second.timer, &m_Players, &m_PlayerLock, NULL, &stopNow);
		C_Thread* run=new C_Thread(TCPReceive, (void*)data);
		localdata.second.lock.M_Lock();
		m_ClientLock.M_Lock();
		m_Clients.push_back(localdata);
		m_ClientLock.M_Unlock();
		localdata.second.lock.M_Unlock();
		localdata.first=run;
	}
	
	void TcpServer::M_ParseClient(TcpSocket* client)
	{
		C_Packet p;
		if(client->Receive(p))
		{
			uchar header; p>>header;
			if(header==TCPConnect)
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
					M_NewPlayer(client);
					M_UpdatePlayers(m_Players, m_PlayerLock);
				}	
			}
		}
	}
	
	void TcpServer::M_DeleteDisconnected()
	{
		m_ClientLock.M_Lock();
		t_Clients::iterator it=m_Clients.begin();
		std::vector<ushort> discoids;
		while(it!=m_Clients.end())
		{
			it->second.lock.M_Lock();
			bool closed=it->second.socket->M_Closed();
			uint64 lastbeat=it->second.timer->M_Get();
			it->second.lock.M_Unlock();
			if(closed || lastbeat>TIMEOUTMS)
			{
				it->second.lock.M_Lock();
				if(it->first) it->first->M_Join();
				std::cout << "Removed disconnected client from clients." << std::endl;
				std::vector<C_Player *>::iterator pt=m_Players.begin();
				while(pt!=m_Players.end())
				{
					C_ServerPlayer* plr=dynamic_cast<C_ServerPlayer*>(*pt);
					if(plr->m_Tcp->M_Id()==it->second.socket->M_Id())
					{
						ushort id=plr->m_Tcp->M_Id();
						std::cout << "Deleting player " << id << std::endl;
						delete plr->m_Packet;
						delete plr;
						std::vector<C_Player *>::iterator del=pt;
						++pt;
						m_PlayerLock.M_Lock();
						m_Players.erase(del);
						m_PlayerLock.M_Unlock();
						break;
					}
					else ++pt;
				}
				discoids.push_back(it->second.socket->M_Id());
				delete it->first;
				delete it->second.timer;
				delete it->second.socket;
				it->second.lock.M_Unlock();
				t_Clients::iterator prev=it;
				++it;
				m_Clients.erase(prev);
				continue;
			}
			else ++it;
		}
		for(std::vector<ushort>::const_iterator di=discoids.begin(); di!=discoids.end(); ++di)
		{
			for(std::vector<C_Player *>::iterator rp=m_Players.begin(); rp!=m_Players.end(); ++rp)
			{
				*(*rp)->m_Packet << ClientDisconnected << *di;
				(*rp)->M_Send();
			}
		}
		m_ClientLock.M_Unlock();
	}
	
	void TcpServer::M_ClearPlayers()
	{
		C_Lock l(m_PlayerLock);
		for(std::vector<C_Player*>::iterator it=m_Players.begin(); it!=m_Players.end(); ++it)
		{
			C_ServerPlayer* plr=dynamic_cast<C_ServerPlayer*>(*it);
			delete plr->m_Packet;
			delete plr;
		}
	}
	
	PWSKOAG_API void TcpServer::ServerLoop()
	{
		m_TcpListener.Bind();
		m_TcpListener.Listen();
		Selector selector;
		selector.Add(m_TcpListener);
		while(!stopNow)
		{
			selector.Wait(TICK_WAITTIME_TCP);
			if(selector.IsReady(m_TcpListener))
			{
				TcpSocket* client = m_TcpListener.Accept();
				if(client) M_ParseClient(client);
			}
			M_DeleteDisconnected();
		}
		m_TcpListener.Close();
		
		if(m_Clients.size()>0) std::cout << "There were " << m_Clients.size() << " clients connected." << std::endl;
		M_ClearPlayers();
		std::cout << "Shut down successful." << std::endl;
	}
	
	PWSKOAG_API void UdpServer::ServerLoop()
	{
		const t_Clients& c = master->GetClients();
		C_Packet p;
		Selector sel;
		while(!stopNow)
		{
			sel.Clear();
			sel.Add(udpSocket);
			if(sel.Wait(TICK_WAITTIME_UDP))
			{
				if(sel.IsReady(udpSocket))
				{
					p.M_Clear();
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
										if(header==UDPConnect)
										{
											ushort id;
											p>>id;
											if(id==s->M_Id())
											{
												s->M_UdpPort(port);
												std::cout << "Bound id " << id << " to UDP port " << port << std::endl;
												p.M_Clear();
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
									while(p.M_Size())
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
												C_Packet p;
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
}