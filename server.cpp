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
	if(!m->M_Load("ground", "ground.2dmodel")) exit(1);
	if(!m->M_Load("box", "box.2dmodel")) exit(1);

	C_Entity* e=p->M_CreateDynamicEntity(m->M_Get("triangle"), 0.15f);
	C_Entity* g=p->M_CreateStaticEntity(m->M_Get("ground"));
	std::vector<C_Entity*> boxes;
	float x=-1.0f;
	for(int i=0; i<16; ++i)
	{
		C_Entity* b=p->M_CreateDynamicEntity(m->M_Get("box"), 0.05f);
		b->M_SetPosition(x, 1.0f);
		x+=0.15f;
		boxes.push_back(b);
	}
	/*
	for(int i=0; i<16; ++i)
	{
		C_Entity* b=p->M_CreateDynamicEntity(m->M_Get("box"), 0.05f);
		b->M_SetPosition(x, 0.8f);
		x+=0.15f;
		boxes.push_back(b);
	}
	for(int i=0; i<16; ++i)
	{
		C_Entity* b=p->M_CreateDynamicEntity(m->M_Get("box"), 0.05f);
		b->M_SetPosition(x, 0.6f);
		x+=0.15f;
		boxes.push_back(b);
	}
	*/
	e->M_SetPosition(0,0);
	g->M_SetPosition(0,-0.8f);

	C_IpAddress ip; unsigned short port;
	C_Packet packet;

	while(run)
	{
		t->M_Reset();
		if(sock.M_Receive(packet, 0, &ip, &port))
		{
			C_Connection* c = pool.M_Exists(ip,port);
			if(c)
			{
				int header;
				packet >> header;
				if(header == NET::Disconnect) pool.M_Remove(c);
			}
			else
			{
				packet.M_Clear();
				pool.M_Add(new C_Connection(ip,port));
				m->M_Get("triangle") >> packet;
				m->M_Get("ground") >> packet;
				m->M_Get("box") >> packet;
				*g >> packet;
				/*
				for(std::vector<C_Entity*>::const_iterator it=boxes.begin(); it!=boxes.end(); ++it)
				{
					*(*it) >> packet;
				}
				*/
				sock.M_Send(packet, ip, port);
			}
		}
		packet.M_Clear();
		*e >> packet;
		for(std::vector<C_Entity*>::const_iterator it=boxes.begin(); it!=boxes.end(); ++it)
		{
			*(*it) >> packet;
			b2Vec2 pos=(*it)->M_Body()->GetPosition();
			if(pos.y<-20.0f) (*it)->M_Body()->SetTransform(b2Vec2(0.0f, 1.0f), 0);
		}
		pool.M_SendToAll(sock, packet);
		packet.M_Clear();
		g_Sleep(33-((int)t->M_Get()*1000));
		p->M_Simulate();
	}
	C_Singleton::M_DestroySingletons();
}
