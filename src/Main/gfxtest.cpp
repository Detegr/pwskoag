#include <Graphics/Graphics.h>
#include <Network/Network.h>
#include <Game/Player.h>
#include <vector>

int main()
{
	pwskoag::C_Renderer r(640,480);
	pwskoag::TcpClient c;
	pwskoag::C_ClientPlayer p(&c);
	r.M_AddObject(*p.M_GetDRAW()->M_GetDrawableObj());
	bool key=false;
	try
	{
		c.M_Connect("localhost", 55555);
	}
	catch(...)
	{
		std::cout << "Failed to connect..." << std::endl;
	}
	while(r.M_Running())
	{
		r.M_Draw();
		switch(r.M_GetEvent().Type)
		{
			case sf::Event::Closed: r.M_Stop(); break;
			case sf::Event::TextEntered:
			{
				sf::Uint32 e=r.M_GetEvent().Text.Unicode;
				if(e==278) {p.M_Send(); std::string str; p.M_SetStr(str);}
				else
				{
					std::string str;
					if(e==277) e=' ';
					str+=(char)e;
					p.M_AddStr(str);
				}
			}
			case sf::Event::KeyReleased: key=false; break;
		}
		pwskoag::msSleep(1000/30);
	}
	c.M_Disconnect();
}
