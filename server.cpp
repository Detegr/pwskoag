#include <iostream>
#include <stdexcept>
#include "serversingleton.h"
#include "ConnectionManager.h"
#include "networkenum.h"
#include "contactlistener.h"

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
	try {sock.M_Bind();} catch(const std::runtime_error& e) {std::cerr << "Failed to bind socket!" << std::endl; return 1;}

	bool run=true;

	C_ConnectionPool pool;
	C_ContactListener contactlistener;
	C_PhysicsManager* p = C_Singleton::M_PhysicsManager();
	p->M_SetContactListener(new C_ContactListener);
	C_Timer* t = C_Singleton::M_Timer();

	C_ModelManager* m = C_Singleton::M_ModelManager();
	if(!m->M_Load("triangle", "test.2dmodel")) exit(1);
	if(!m->M_Load("horizwall", "horizwall.2dmodel")) exit(1);
	if(!m->M_Load("vertwall", "vertwall.2dmodel")) exit(1);
	if(!m->M_Load("box", "box.2dmodel")) exit(1);
	if(!m->M_Load("bullet", "bullet.2dmodel")) exit(1);

	C_Entity* top=p->M_CreateStaticEntity(m->M_Get("horizwall"), 2.0f);
	C_Entity* bottom=p->M_CreateStaticEntity(m->M_Get("horizwall"), 2.0f);
	C_Entity* left=p->M_CreateStaticEntity(m->M_Get("vertwall"), 2.0f);
	C_Entity* right=p->M_CreateStaticEntity(m->M_Get("vertwall"), 2.0f);

	std::vector<C_Entity*> boxes;
	std::vector<C_Entity*> players;

	float x=-1.0f;
	float y=0.6f;
	float size=0.08f;
	int rows=4;
	for(int j=0; j<rows; ++j)
	{
		x=-1.0f;
		for(int i=0; i<4; ++i) {
			C_Entity* b=p->M_CreateDynamicEntity(m->M_Get("box"), size);
			b->SetPosition(x, y);
			x+=0.15f;
			boxes.push_back(b);
		}
		size-=0.015f;
		y-=2.0f/rows;
	}

	top->SetPosition(0,2.0f);
	bottom->SetPosition(0,-2.0f);
	left->SetPosition(-2.0f,0.0f);
	right->SetPosition(2.0f,0.0f);

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
					else if(header & 0x10)
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
				e->SetPosition(0,0);
				c->M_SetEntity(e);
				m->M_Get("triangle") >> packet;
				m->M_Get("horizwall") >> packet;
				m->M_Get("vertwall") >> packet;
				m->M_Get("box") >> packet;
				m->M_Get("bullet") >> packet;

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
				packet << (unsigned char)NET::PlayerId << e->M_Id();
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
		g_Sleep(250-((int)t->M_Get()*1000));
		p->M_Simulate();
	}
	C_Singleton::M_DestroySingletons();
}
