#include <iostream>
#include <stdexcept>
#include "serversingleton.h"
#include "ConnectionManager.h"
#include "networkenum.h"

using namespace dtglib;

inline void g_Sleep(unsigned int ms)
{
	#ifdef _WIN32
		Sleep(ms);
	#else
		struct timespec ts;
		if(ms==0){sched_yield(); return;}
		else
		{
			ts.tv_sec=ms/1000;
			ts.tv_nsec=ms%1000*1000000;
		}
		nanosleep(&ts, NULL);
	#endif
}

int main()
{
	C_UdpSocket sock(51119);
	sock.M_Bind();
	//C_Thread connectionhandler(&handleconnections, &sock);

	bool run=true;

	C_ConnectionPool pool;
	C_PhysicsManager* p = C_Singleton::M_PhysicsManager();
	C_Timer* t = C_Singleton::M_Timer();

	C_ModelManager* m = C_Singleton::M_ModelManager();
	if(!m->M_Load("triangle", "test.2dmodel")) exit(1);
	if(!m->M_Load("horizwall", "horizwall.2dmodel")) exit(1);
	if(!m->M_Load("vertwall", "vertwall.2dmodel")) exit(1);
	if(!m->M_Load("box", "box.2dmodel")) exit(1);

	C_Entity* top=p->M_CreateStaticEntity(m->M_Get("horizwall"), 2.0f);
	C_Entity* bottom=p->M_CreateStaticEntity(m->M_Get("horizwall"), 2.0f);
	C_Entity* left=p->M_CreateStaticEntity(m->M_Get("vertwall"), 2.0f);
	C_Entity* right=p->M_CreateStaticEntity(m->M_Get("vertwall"), 2.0f);
	std::vector<C_Entity*> boxes;
	std::vector<C_Entity*> players;

	float x=-1.0f;
	float y=0.5f;
	for(int i=0; i<16; ++i)
	{
		C_Entity* b=p->M_CreateDynamicEntity(m->M_Get("box"), 0.05f);
		b->M_SetPosition(x, y);
		x+=0.15f;
		boxes.push_back(b);
	}
	x=-1.0f;
	for(int i=0; i<16; ++i)
	{
		C_Entity* b=p->M_CreateDynamicEntity(m->M_Get("box"), 0.04f);
		b->M_SetPosition(x, y-0.4f);
		x+=0.15f;
		boxes.push_back(b);
	}
	x=-1.0f;
	for(int i=0; i<16; ++i)
	{
		C_Entity* b=p->M_CreateDynamicEntity(m->M_Get("box"), 0.03f);
		b->M_SetPosition(x, y-0.8f);
		x+=0.15f;
		boxes.push_back(b);
	}

	top->M_SetPosition(0,1.0f);
	bottom->M_SetPosition(0,-1.0f);
	left->M_SetPosition(-1.6f,0.0f);
	right->M_SetPosition(1.6f,0.0f);

	C_IpAddress ip; unsigned short port;
	C_Packet packet;
	C_Packet newplayers;

	while(run)
	{
		t->M_Reset();
		while(sock.M_Receive(packet, 1, &ip, &port))
		{
			C_Connection* c = pool.M_Exists(ip,port);
			if(c)
			{
				unsigned char header=0;
				packet >> header;
				if(c->M_Pending())
				{
					if(header == NET::Connect)
					{
						c->M_Pending(false);
					}
				}
				else
				{
					if(header == NET::Disconnect)
					{
						packet.M_Clear();
						for(std::vector<C_Entity*>::iterator it=players.begin(); it!=players.end(); ++it)
						{
							if((*it) == c->M_GetEntity())
							{
								players.erase(it);
								break;
							}
						}
						packet << (unsigned char)NET::EntityDeleted << c->M_GetEntity()->M_Id();
						pool.M_Remove(c);
					}
					else if((header & 0xF0) == 0xF0)
					{
						c->M_SetKeys(header);
					}
				}
			}
			else
			{
				packet.M_Clear();
				c=pool.M_Add(new C_Connection(ip,port));
				C_Entity* e=p->M_CreateDynamicEntity(m->M_Get("triangle"), 0.08f);
				e->M_SetPosition(0,0);
				c->M_SetEntity(e);
				m->M_Get("triangle") >> packet;
				m->M_Get("horizwall") >> packet;
				m->M_Get("vertwall") >> packet;
				m->M_Get("box") >> packet;
				top->M_DumpFullInstance(packet);
				bottom->M_DumpFullInstance(packet);
				left->M_DumpFullInstance(packet);
				right->M_DumpFullInstance(packet);
				e->M_DumpFullInstance(packet);
				e->M_DumpFullInstance(newplayers);
				for(std::vector<C_Entity*>::const_iterator it=players.begin(); it!=players.end(); ++it)
				{
					(*it)->M_DumpFullInstance(packet);
				}
				for(std::vector<C_Entity*>::const_iterator it=boxes.begin(); it!=boxes.end(); ++it)
				{
					(*it)->M_DumpFullInstance(packet);
				}
				sock.M_Send(packet, ip, port);
				packet.M_Clear();
				players.push_back(e);
			}
		}
		for(std::vector<C_Entity*>::const_iterator it=players.begin(); it!=players.end(); ++it)
		{
			*(*it) >> packet;
		}
		for(std::vector<C_Entity*>::const_iterator it=boxes.begin(); it!=boxes.end(); ++it)
		{
			*(*it) >> packet;
		}
		pool.M_SendToAll(sock, packet);
		if(newplayers.M_Size())
		{
			pool.M_SendToAll(sock, newplayers);
			newplayers.M_Clear();
		}
		packet.M_Clear();
		g_Sleep(33-((int)t->M_Get()*1000));
		p->M_Simulate();
	}
	C_Singleton::M_DestroySingletons();
}
