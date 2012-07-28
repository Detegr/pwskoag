#include <Util/Version.h>
#include <Util/DataUtils.h>
#include "Netcommand.h"
#include "Server.h"

using namespace dtglib;

namespace pwskoag
{
	PWSKOAG_API void C_Server::M_Start()
	{
		C_Lock lock(m_SelfMutex);
		if(!m_SelfThread)
		{
			m_StopNow=false;
			m_SelfThread = new C_Thread(C_Server::ServerInitializer, this);
		}
		else std::cerr << "Server already running!" << std::endl;
	}
	PWSKOAG_API void C_Server::M_Stop()
	{
		C_Lock lock(m_SelfMutex);
		m_StopNow=true;
		if(m_SelfThread) {m_SelfThread->M_Join(); delete m_SelfThread; m_SelfThread=NULL;}
		else std::cerr << "Server already stopped!" << std::endl;
	}
	PWSKOAG_API void C_Server::M_ForceStop()
	{
		//Lock lock(selfMutex);
		//if(selfThread) {selfThread->Terminate(); delete selfThread; selfThread=NULL;}
	}
	void C_Server::ServerInitializer(void* args)
	{
		C_Server* s = (C_Server*)args;
		s->ServerLoop();
	}
	PWSKOAG_API C_Server::~C_Server()
	{
		if(m_SelfThread) std::cerr << "!! Server not stopped and still shutting down !!" << std::endl;
	}
	
	C_TcpServer::~C_TcpServer()
	{
		C_Lock lock(m_SelfMutex);
		for(t_Clients::iterator it=m_Clients.begin(); it!=m_Clients.end(); ++it) delete it->first;
	}
	
	static void M_UpdatePlayers(const t_Entities& plrs, C_Mutex& lock)
	{
		C_Lock l(lock);
		
		for(t_Entities::const_iterator it=plrs.begin(); it!=plrs.end(); ++it)
		{
			C_ServerPlayer* plr=static_cast<C_ServerPlayer*>(*it);
			for(t_Entities::const_iterator it2=plrs.begin(); it2!=plrs.end(); ++it2)
			{
				C_ServerPlayer* plr2=static_cast<C_ServerPlayer*>(*it2);
				*(plr->m_Packet)
				<<(uchar)C_Netcommand::Connect
				<<plr2->M_Id();
				//<<plr2->M_Position();
			}
			plr->M_Send();
			plr->m_Packet->M_Clear();			
		}
	}
	
	static bool M_ParsePacket(C_Packet& p, C_Netcommand::e_Command header, C_ThreadData* data)
	{
		C_Mutex* lock=data->m_Lock;
		C_Mutex* plrlock=data->m_PlayerLock;
		C_Timer* timer=data->m_Timer;
		t_Entities* plrs=data->m_Players;
		C_TcpSocket* client=(C_TcpSocket*)data->m_Socket;
		
		if(header==C_Netcommand::Update)
		{
			/*
			 * Message:
			 * [int|std::string]
			 */
			/*
			int i; p>>i;
			std::string str; p>>str;
			std::cout << "ID: " << i << ": " << str << std::endl;
			lock->M_Lock();
			plrlock->M_Lock();
			C_Entity* e = g_EntityById(*plrs, i);
			if(e)
			{
				C_ServerPlayer* plr = static_cast<C_ServerPlayer*>(e);
				plr->M_SetStr(str);
				plr->m_Time.M_Reset();
			}
			plrlock->M_Unlock();
			M_UpdatePlayers(*plrs, *plrlock);
			lock->M_Unlock();
			*/
		}
		else if(header==C_Netcommand::HeartBeat)
		{
			lock->M_Lock();
			std::cout << "Time from last beat: " << timer->M_Get() << std::endl;
			timer->M_Reset();
			std::cout << "Time now: " << timer->M_Get() << std::endl;
			lock->M_Unlock();
			std::cout << "Beat from " << client->M_Ip() << ":" << client->M_Port() << std::endl;
		}
		else if(header==C_Netcommand::Connect)
		{
			lock->M_Lock();
			client->M_Clear();
			lock->M_Unlock();
			std::cout << "Client disconnected." << std::endl;
			return false;
		}
		else if(header==C_Netcommand::EOP) return true;
		else throw std::runtime_error("Invalid packet header.");
		return true;
	}
	
	static void TCPReceive(void* args)
	{
		C_ThreadData* data=(C_ThreadData*)args;
		C_Mutex* lock=data->m_Lock;
		C_TcpSocket* client=(C_TcpSocket*)data->m_Socket;
		bool* stopNow=data->m_StopNow;
		
		C_Selector s;
		C_Packet p;
		while(!*stopNow)
		{
			s.M_Clear();
			s.M_Add(*client);
			s.M_Wait(4000);
			if(s.M_IsReady(*client))
			{
				if(client->M_Receive(p))
				{
					uchar header=0;
					while(p.M_Size())
					{
						p>>header;
						if(!M_ParsePacket(p, (C_Netcommand::e_Command)header, data)) {delete data; return;}
					}
				}
				else
				{
					lock->M_Lock();
					client->M_Clear();
					lock->M_Unlock();
					std::cout << "Client disconnected?" << std::endl;
					break;
				}
			}
			else
			{
				lock->M_Lock();
				client->M_Clear();
				lock->M_Unlock();
				std::cout << "Client timed out" << std::endl;
				break;
			}
		}
		delete data;
	}
	
	void C_TcpServer::M_GenerateId(C_TcpSocket* client)
	{
		bool ok=false;
		do
		{
			ok=true;
			ushort id=rand()%(((ushort)1)<<15);
			m_ClientLock.M_Lock();
			for(t_Clients::iterator it=m_Clients.begin(); it!=m_Clients.end(); ++it)
			{
				if(it->second.m_Socket->M_Id()==id) {ok=false; break;}
			}
			m_ClientLock.M_Unlock();
			client->M_Id(id);
		} while(!ok);
		std::cout << "Generated id: " << client->M_Id() << std::endl;
	}
	
	void C_TcpServer::M_NewPlayer(C_TcpSocket* client)
	{
		M_GenerateId(client);
		
		std::cout << "Client connected" << std::endl;
		C_ServerPlayer* newplayer=new C_ServerPlayer(client, new C_Packet);
		
		newplayer->M_Id(client->M_Id());
		*newplayer->m_Packet << C_Netcommand::Connect << client->M_Id();
		
		m_PlayerLock.M_Lock();
		m_Players.push_back(newplayer);
		m_PlayerLock.M_Unlock();
		
		std::cout << "Players: " << m_Players.size() << std::endl;
		
		C_LocalThreadData localdata(client);
		C_ThreadData* data=new C_ThreadData(
											&localdata.m_Lock,
											client,
											localdata.m_Timer,
											&m_Players,
											&m_PlayerLock,
											NULL,
											&m_StopNow
											);
		C_Thread* run=new C_Thread(TCPReceive, (void*)data);
		M_NewClient(localdata, run);
	}
	
	void C_TcpServer::M_NewClient(C_LocalThreadData& localdata, C_Thread* thread)
	{
		localdata.m_Lock.M_Lock();
		m_ClientLock.M_Lock();
		m_Clients.push_back(std::make_pair(thread, localdata));
		m_ClientLock.M_Unlock();
		localdata.m_Lock.M_Unlock();
	}
	
	void C_TcpServer::M_ParseClient(C_TcpSocket* client)
	{
		C_Packet p;
		if(client->M_Receive(p))
		{
			uchar header; p>>header;
			if(header==C_Netcommand::Connect)
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
	
	void C_TcpServer::M_DeleteDisconnected()
	{
		// This method is _ugly_ :G
		m_ClientLock.M_Lock();
		t_Clients::iterator it=m_Clients.begin();
		std::vector<ushort> discoids;
		while(it!=m_Clients.end())
		{
			it->second.m_Lock.M_Lock();
			bool closed=it->second.m_Socket->M_Closed();
			uint64 lastbeat=it->second.m_Timer->M_Get();
			it->second.m_Lock.M_Unlock();
			if(closed || lastbeat>TIMEOUTMS)
			{
				it->second.m_Lock.M_Lock();
				if(it->first) it->first->M_Join();
				std::cout << "Removed disconnected client from clients." << std::endl;
				t_Entities::iterator pt=m_Players.begin();
				while(pt!=m_Players.end())
				{
					C_ServerPlayer* plr=static_cast<C_ServerPlayer*>(*pt);
					if(plr->m_Tcp->M_Id()==it->second.m_Socket->M_Id())
					{
						ushort id=plr->m_Tcp->M_Id();
						std::cout << "Deleting player " << id << std::endl;
						delete plr->m_Packet;
						delete plr;
						t_Entities::iterator del=pt;
						++pt;
						m_PlayerLock.M_Lock();
						m_Players.erase(del);
						m_PlayerLock.M_Unlock();
						break;
					}
					else ++pt;
				}
				discoids.push_back(it->second.m_Socket->M_Id());
				delete it->first;
				delete it->second.m_Timer;
				delete it->second.m_Socket;
				it->second.m_Lock.M_Unlock();
				t_Clients::iterator prev=it;
				++it;
				m_Clients.erase(prev);
				continue;
			}
			else ++it;
		}
		for(std::vector<ushort>::const_iterator di=discoids.begin(); di!=discoids.end(); ++di)
		{
			for(t_Entities::iterator rp=m_Players.begin(); rp!=m_Players.end(); ++rp)
			{
				*(*rp)->m_Packet << C_Netcommand::Disconnect << *di;
				(*rp)->M_Send();
			}
		}
		m_ClientLock.M_Unlock();
	}
	
	void C_TcpServer::M_ClearPlayers()
	{
		C_Lock l(m_PlayerLock);
		for(t_Entities::iterator it=m_Players.begin(); it!=m_Players.end(); ++it)
		{
			C_ServerPlayer* plr=static_cast<C_ServerPlayer*>(*it);
			delete plr->m_Packet;
			delete plr;
		}
	}
	
	PWSKOAG_API void C_TcpServer::ServerLoop()
	{
		m_TcpListener.M_Bind();
		m_TcpListener.M_Listen();
		C_Selector selector;
		selector.M_Add(m_TcpListener);
		while(!m_StopNow)
		{
			selector.M_Wait(TICK_WAITTIME_TCP);
			if(selector.M_IsReady(m_TcpListener))
			{
				C_TcpSocket* client = m_TcpListener.M_Accept();
				if(client) M_ParseClient(client);
			}
			M_DeleteDisconnected();
		}
		m_TcpListener.M_Close();
		
		if(m_Clients.size()>0) std::cout << "There were " << m_Clients.size() << " clients connected." << std::endl;
		M_ClearPlayers();
		std::cout << "Shut down successful." << std::endl;
	}
	
	static void M_NewPlayer(C_Selector& sel, C_Packet& p, const C_IpAddress& ip, ushort port, C_TcpSocket* s, C_UdpSocket* us)
	{
		/*
		for(;;)
		{
			uchar header=0;
			p >> header;
			if(header==C_Netcommand::Connect)
			{
				ushort id;
				p>>id;
				if(id==s->M_Id())
				{
					// Potential multithread problem.
						s->M_UdpPort(port);
					// !!!
					std::cout << "Bound id " << id << " to UDP port " << port << std::endl;
					p.M_Clear();
					p<<HandShake<<EOP;
					if(sel.WaitWrite(TICK_WAITTIME_UDP))
					{
						if(sel.IsReady(*us))
						{
							us->Send(p, ip, port);
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
		*/
	}
	
	static void M_ParsePacket(const C_IpAddress& ip, ushort port, C_Packet& p)
	{
		/*
		uchar header=0;
		while(p.M_Size())
		{
			p>>header;
			if(header==String)
			{
				std::string str; p>>str;
				std::cout << str << std::endl;
			}
			else if(header==EOP) break;
			else if(header==MessageTimer)
			{
				ushort id;
				p>>id;
			}
			else
			{
				throw std::runtime_error("Invalid UDP packet header.");
			}
		}
		*/
	}
	
	static void UDPReceive(void* args)
	{
		/*
		C_ThreadData* data=static_cast<C_ThreadData*>(args);
		C_TcpServer* m_Master=static_cast<C_TcpServer *>(data->m_Void1);
		C_UdpSocket* s=static_cast<C_UdpSocket *>(data->socket);
		bool* stopNow=data->stopNow;
		C_Packet p;
		C_Selector sel;
		while(!*stopNow)
		{
			sel.M_Clear();
			sel.M_Add(*s);
			if(sel.M_Wait(TICK_WAITTIME_UDP))
			{
				if(sel.M_IsReady(*s))
				{
					p.M_Clear();
					C_IpAddress ip;
					ushort port;
					if(s->M_Receive(p, &ip, &port))
					{
						m_Master->M_ClientLock(true);
						const t_Clients& c=m_Master->M_GetClients();
						for(t_Clients::const_iterator it=c.begin(); it!=c.end(); ++it)
						{
							C_TcpSocket* tcps=static_cast<C_TcpSocket*>(it->second.socket);
							if(ip==tcps->GetIp())
							{
								if(!tcps->M_UdpPort())
								{
									M_NewPlayer(sel,p,ip,port,tcps,s);
									continue;
								}
								if(tcps->M_UdpPort()==port)
								{
									M_ParsePacket(ip,port,p);
									break;
								}
							}
						}
						m_Master->M_ClientLock(false);
					}
					else std::cerr << "Data wasn't ready." << std::endl;
				}
			}
		}
		*/
	}
	void C_UdpServer::M_UpdateGamestate(C_Packet& p)
	{
		/*
		m_Master->M_PlayerLock(true);
		t_Entities& plrs=m_Master->M_Players();
		for(t_Entities::iterator it=plrs.begin(); it!=plrs.end(); ++it)
		{
			for(t_Entities::iterator pt=plrs.begin(); pt!=plrs.end(); ++pt)
			{
				C_ServerPlayer* plr=static_cast<C_ServerPlayer *>(*pt);
				(*plr->m_Packet)<<C_Netcommand::Update<<plr->M_Id()<<plr->M_Position();
			}
			(*it)->M_SendUdp(udpSocket);
		}
		m_Master->M_PlayerLock(false);
		*/
	}
	
	PWSKOAG_API void C_UdpServer::ServerLoop()
	{
		/*
		C_ThreadData data(NULL, &m_UdpSocket, NULL, NULL, NULL, NULL, &m_StopNow, m_Master);
		C_Thread t(UDPReceive, &data);
		C_Packet p;
		while(!stopNow)
		{
			M_UpdateGamestate(p);
			g_Sleep(TICK_WAITTIME_UDP);
		}
		*/
	}
}
