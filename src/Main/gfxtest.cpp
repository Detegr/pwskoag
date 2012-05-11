#include <Graphics/Graphics.h>
#include <Network/Client.h>
#include <Game/Player.h>
#include <Game/ClientPlayer.h>
#include <Game/Keyboard.h>
#include <vector>

int main()
{
	pwskoag::TcpClient c;
	pwskoag::UdpClient u;
	pwskoag::C_ClientPlayer* p;
	pwskoag::C_ClientPlayer fallbackp;
	try
	{
		if(c.M_Connect("localhost", 55555))
		{
			p=c.M_OwnPlayer();
			u.M_Initialize(&c);
			u.M_Connect("localhost", 55556);
		}
		else throw std::runtime_error("fuu");
	}
	catch(...)
	{
		p=&fallbackp;
		std::cout << "Failed to connect..." << std::endl;
	}
	pwskoag::C_Renderer r(640,480, c.M_Players());
	{
		while(r.M_Running())
		{
			c.M_PlayerLock(true);
			r.M_Draw();
			c.M_PlayerLock(false);
			r.M_UpdateEvent();
			if(r.M_GetEvent().Type==sf::Event::Closed) break;
			pwskoag::g_Sleep(1);
		}
	}
	u.M_Disconnect();
	c.M_Disconnect();
}
