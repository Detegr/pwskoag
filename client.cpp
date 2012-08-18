#include "singleton.h"
#include "entity.h"
#include "dtglib/Network.h"
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

bool M_DoConnection(C_UdpSocket& sock)
{
	C_Packet p;
	p << NET::Connect;
	sock.M_Send(p);
	p.M_Clear();
	C_IpAddress ip;
	ushort port;
	if(sock.M_Receive(p, 2000, &ip, &port))
	{
		return (sock.M_Ip() == ip) && (sock.M_Port() == port);
	}
	return false;
}

int main()
{
	C_UdpSocket sock("localhost", 51119);
	if(!M_DoConnection(sock))
	{
		std::cerr << "Failed to connect!" << std::endl;
		C_Singleton::M_DestroySingletons();
		return 1;
	}
	C_Renderer* r = C_Singleton::M_Renderer();
	C_ShaderManager* s = C_Singleton::M_ShaderManager();
	C_ModelManager* m = C_Singleton::M_ModelManager();

	if(!m->M_Load("triangle", "test.2dmodel")) exit(1);
	if(!m->M_Load("ground", "ground.2dmodel")) exit(1);
	if(!m->M_Load("box", "box.2dmodel")) exit(1);

	s->M_Load("minimal");
	r->M_Use(s->M_Get("minimal"));

	C_GfxEntity* e=C_GfxEntity::M_Create(*m->M_Get("triangle"), 0.1f);
	C_GfxEntity* g=C_GfxEntity::M_Create(*m->M_Get("ground"));

	g->M_SetPosition(0.0f, -0.4f);

	bool running=true;
	while(running)
	{
		C_Packet p;
		sock.M_Receive(p);
		float x=0.0f;
		float y=0.0f;
		p >> x;
		p >> y;
		std::cout << x << " " << y << std::endl;
		e->M_SetPosition(x,y);

		g_Sleep(1);
		running=!(C_Singleton::M_InputHandler()->M_Get(ESC));
		r->M_Draw();
	}
	C_Singleton::M_DestroySingletons();

	return 0;
}
