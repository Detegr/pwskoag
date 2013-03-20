#include "singleton.h"
#include "entity.h"
#include "dtglib/Network.h"
#include "networkenum.h"
#include "packetparser.h"
#include "input.h"
#include "timer.h"

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
	p << (unsigned char)NET::Connect;
	sock.M_Send(p);
	p.M_Clear();
	C_IpAddress ip;
	ushort port;
	if(sock.M_Receive(p, 2000, &ip, &port))
	{
		if((sock.M_Ip() == ip) && (sock.M_Port() == port))
		{
			while(p.M_Size()) C_PacketParser::M_Parse(p);
			p.M_Clear();
			p << (unsigned char)NET::Connect;
			return sock.M_Send(p);
		}
	}
	return false;
}

unsigned char getkeys()
{
	unsigned char keyvec=0x10;
	C_InputHandler* i = C_Singleton::M_InputHandler();
	if(i->M_Get(Key::SPACE)) keyvec|=0x20;
	if(i->M_Get(Key::UP)) keyvec |= 0x8;
	else if(i->M_Get(Key::DOWN)) keyvec |=0x4;
	if(i->M_Get(Key::LEFT)) keyvec |= 0x2;
	else if(i->M_Get(Key::RIGHT)) keyvec |= 0x1;
	return keyvec;
}

int main()
{
	#ifdef _WIN32
		C_SocketInitializer si;
	#endif
	C_IpAddress ip("127.0.0.1");
	C_UdpSocket sock(ip, 51119);
	if(!M_DoConnection(sock))
	{
		std::cerr << "Failed to connect to " << ip << "!" << std::endl;
		C_Singleton::M_DestroySingletons();
		return 1;
	}

	C_Packet p;
	C_Singleton::M_InputHandler();
	C_Renderer* r = C_Singleton::M_Renderer();
	C_ShaderManager* s = C_Singleton::M_ShaderManager();

	s->M_Load("minimal");
	s->M_Load("green");
	r->UseShader(s->M_Get("minimal"));

	bool running=true;
	p.M_Clear();

	unsigned char keyvec=0;
	unsigned char prevkeyvec=0;
	C_Timer idt;
	idt.M_Reset();
	while(running)
	{
		if(sock.M_Receive(p, 0, NULL, NULL))
		{
			idt.M_Reset();
			C_Packet keys;
			keyvec=getkeys();
			if(prevkeyvec != 0xF0)
			{
				keys << keyvec;
				sock.M_Send(keys);
			}
			prevkeyvec=keyvec;
			while(p.M_Size()) C_PacketParser::M_Parse(p);
		}
		else
		{
			const std::vector<C_GfxEntity*>& entities = r->Entities();
			for(std::vector<C_GfxEntity*>::const_iterator it=entities.begin(); it!=entities.end(); ++it)
			{
				(*it)->M_ExtrapolatePosition(idt.M_Get());
				(*it)->M_ExtrapolateRotation(idt.M_Get());
			}
		}
		g_Sleep(1);
		p.M_Clear();
		r->Draw();

		running=!(C_Singleton::M_InputHandler()->M_Get(ESC));
	}
	p.M_Clear();
	p << (unsigned char)NET::Disconnect;
	sock.M_Send(p);

	C_Singleton::M_DestroySingletons();

	return 0;
}
