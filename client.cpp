#include "singleton.h"
#include "entity.h"
#include "dtglib/Network.h"
#include "networkenum.h"
#include "packetparser.h"

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
		if((sock.M_Ip() == ip) && (sock.M_Port() == port))
		{
			int header;
			p >> header;
			while(header == NET::ModelBegin && p.M_Size())
			{
				C_PacketParser<C_Model>::M_Parse(p);
				if(p.M_Size()) p >> header;
			}
			return true;
		}
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

	s->M_Load("minimal");
	r->M_Use(s->M_Get("minimal"));

	C_GfxEntity* e=C_GfxEntity::M_Create(m->M_Get("triangle"), 0.15f);
	C_GfxEntity* g=C_GfxEntity::M_Create(m->M_Get("ground"));
	C_GfxEntity* b=C_GfxEntity::M_Create(m->M_Get("box"), 0.05f);

	e->M_SetPosition(0.0f, 0.0f);
	g->M_SetPosition(0.0f, -0.8f);
	b->M_SetPosition(0.13f, 1.0f);

	bool running=true;
	while(running)
	{
		C_Packet p;
		sock.M_Receive(p);
		float x=0.0f;
		float y=0.0f;
		float a=0.0f;
		float boxx=0.0f;
		float boxy=0.0f;
		float boxa=0.0f;
		p >> x;
		p >> y;
		p >> a;
		p >> boxx;
		p >> boxy;
		p >> boxa;
		e->M_SetPosition((x/10.0f),(y/10.0f));
		e->M_SetRotation(a);
		b->M_SetPosition(boxx/10.0f,boxy/10.0f);
		b->M_SetRotation(boxa);

		g_Sleep(1);
		running=!(C_Singleton::M_InputHandler()->M_Get(ESC));
		r->M_Draw();
	}
	C_Singleton::M_DestroySingletons();

	return 0;
}
