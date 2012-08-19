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
	C_Entity* b=p->M_CreateDynamicEntity(m->M_Get("box"), 0.05f);
	e->M_SetPosition(0,0);
	g->M_SetPosition(0,-0.8f);
	b->M_SetPosition(0.13, 1.0f);

	C_IpAddress ip; unsigned short port;
	C_Packet packet;

	while(run)
	{
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
				*e >> packet;
				*g >> packet;
				*b >> packet;
				sock.M_Send(packet, ip, port);
			}
		}
		packet.M_Clear();
		t->M_Reset();
		g_Sleep(10);
		p->M_Simulate();
		*e >> packet;
		*b >> packet;
		pool.M_SendToAll(sock, packet);
		packet.M_Clear();
	}
	C_Singleton::M_DestroySingletons();
}
