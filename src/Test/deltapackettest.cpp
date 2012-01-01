#include <Util/Base.h>
#include <Network/Packet.h>

#include <iostream>

int main()
{
	pwskoag::C_Packet p;
	pwskoag::C_Packet p2;
	p << pwskoag::TCPConnect << 1234 << pwskoag::TCPConnect << 2345;
	p2 << pwskoag::TCPConnect << 1234 << pwskoag::TCPConnect << 4567;
	pwskoag::C_DeltaPacket delta(p);
	pwskoag::C_Packet d=delta.M_Delta(p2);
	pwskoag::uint x;
	pwskoag::uchar header;
	d>>header;
	d.M_GetDataChunk(header, &x);
	std::cout << x << std::endl;
	std::cout << d.M_Size() << std::endl;
}
