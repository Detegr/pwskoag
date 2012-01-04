#include <Util/Base.h>
#include <Network/Packet.h>

#include <iostream>

int main()
{
	pwskoag::C_Packet p;
	pwskoag::C_Packet p2;
	pwskoag::C_Packet p3;
	p << pwskoag::TCPConnect << 1234 << pwskoag::TCPConnect << 2345;
	p2 << pwskoag::TCPConnect << 1234 << pwskoag::String << std::string("Deltapacket test") << pwskoag::TCPConnect << 1441;
	p3 << pwskoag::TCPConnect << 1234 << pwskoag::TCPConnect << 4567 << pwskoag::TCPConnect << 8888;
	pwskoag::C_DeltaPacket delta(p);
	pwskoag::C_Packet d=delta.M_Delta(p2);
	pwskoag::uchar h;
	while(d.M_Size())
	{
		d>>h;
		uint u;
		std::string s;
		if(h==pwskoag::TCPConnect) {d>>u; std::cout << u << std::endl;}
		else if(h==pwskoag::String) {d>>s; std::cout << s << std::endl;}
		else { std::cout << "Unknown header: " << (int)h << std::endl; exit(1);}
	}
	d=delta.M_Delta(p3);
	while(d.M_Size())
	{
		d>>h;
		uint u;
		std::string s;
		if(h==pwskoag::TCPConnect) {d>>u; std::cout << u << std::endl;}
		else if(h==pwskoag::String) {d>>s; std::cout << s << std::endl;}
		else { std::cout << "Unknown header: " << (int)h << std::endl; exit(1);}
	}
	/*
	pwskoag::uint x;
	pwskoag::uchar header;
	d>>header;
	d.M_GetDataChunk(header, &x);
	std::cout << x << std::endl;
	std::cout << d.M_Size() << std::endl;
	*/
}
