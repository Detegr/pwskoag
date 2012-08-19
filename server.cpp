#include <iostream>
#include <stdexcept>
#include "serversingleton.h"
#include "ConnectionManager.h"

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
	C_Entity* b=p->M_CreateDynamicEntity(m->M_Get("box"), 0.05f);
	e->M_SetPosition(0,0);
	g->M_SetPosition(0,-0.8f);
	b->M_SetPosition(0.13, 1.0f);

	C_IpAddress ip; unsigned short port;
	C_Packet packet;
	if(sock.M_Receive(packet, &ip, &port))
	{
		packet.M_Clear();
		if(!pool.M_Exists(ip,port))
		{
			pool.M_Add(new C_Connection(ip,port));
			m->M_Get("triangle") >> packet;
			m->M_Get("ground") >> packet;
			m->M_Get("box") >> packet;
			sock.M_Send(packet, ip, port);
		}
	}

	while(run)
	{
		t->M_Reset();
		C_Packet packet;
		g_Sleep(10);
		p->M_Simulate();
		//std::cout << (float)e->M_Body()->GetPosition().x << " " << (float)e->M_Body()->GetPosition().y << " box: " << (float)b->M_Body()->GetPosition().x << " " << (float)b->M_Body()->GetPosition().y << std::endl;
		packet << (float)e->M_Body()->GetPosition().x << (float)e->M_Body()->GetPosition().y << (float)e->M_Body()->GetAngle() << (float)b->M_Body()->GetPosition().x << (float)b->M_Body()->GetPosition().y << b->M_Body()->GetAngle();
		sock.M_Send(packet, pool.M_Head()->m_Ip, pool.M_Head()->m_Port);
	}
	C_Singleton::M_DestroySingletons();
}
