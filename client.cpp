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
			while(p.M_Size()) C_PacketParser<C_DummyParse>::M_Parse(p);
			return true;
		}
	}
	return false;
}

int main()
{
	C_UdpSocket sock("192.168.1.3", 51119);
	if(!M_DoConnection(sock))
	{
		std::cerr << "Failed to connect!" << std::endl;
		C_Singleton::M_DestroySingletons();
		return 1;
	}
	C_Renderer* r = C_Singleton::M_Renderer();
	C_ShaderManager* s = C_Singleton::M_ShaderManager();

	s->M_Load("minimal");
	r->M_Use(s->M_Get("minimal"));

	bool running=true;
	C_Packet p;
	p.M_Clear();
	while(running)
	{
		sock.M_Receive(p);
		while(p.M_Size()) C_PacketParser<C_DummyParse>::M_Parse(p);

		g_Sleep(1);
		running=!(C_Singleton::M_InputHandler()->M_Get(ESC));
		r->M_Draw();
	}
	p.M_Clear();
	p << NET::Disconnect;
	sock.M_Send(p);

	C_Singleton::M_DestroySingletons();

	return 0;
}
