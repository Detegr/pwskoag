#include <Graphics/Graphics.h>
#include <Network/Network.h>
#include <Network/Client.h>
#include <Game/Player.h>
#include <Game/ClientPlayer.h>
#include <Game/Keyboard.h>
#include <vector>

int main()
{
	pwskoag::C_Renderer r(640,480);
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
	r.M_AddObject(*p);
	{
		pwskoag::C_RendererSyncer rs(r,c);
		while(r.M_Running())
		{
			c.M_PlayerLock(true);
			std::vector<pwskoag::C_Player*> plrs=c.M_Players();
			for(std::vector<pwskoag::C_Player*>::iterator it=plrs.begin(); it!=plrs.end(); ++it)
			{
				pwskoag::C_ClientPlayer* plr=dynamic_cast<pwskoag::C_ClientPlayer*>(*it);
				r.M_AddObjectCheckExisting(*plr, c.M_Players(), c.M_GetPlayerLock());
			}
			c.M_PlayerLock(false);
			r.M_Draw();
			r.M_UpdateEvent();
			switch(r.M_GetEvent().Type)
			{
				case sf::Event::Closed: r.M_Stop(); break;
				case sf::Event::TextEntered:
				{
					char c=pwskoag::Keyboard::M_GetChar(r.M_GetEvent().Text.Unicode);				
					if(c==pwskoag::Keyboard::RETURN) {p->M_Send();}
					else if(c==pwskoag::Keyboard::BACKSPACE) {std::string str=p->M_GetStr(); if(str.size()) str.resize(str.size()-1); p->M_SetStr(str);}
					else if(c==0);
					else
					{
						std::string str;
						str+=c;
						p->M_AddStr(str);
					}
				}
			}
			pwskoag::msSleep(1000/30);
		}
	}
	u.M_Disconnect();
	c.M_Disconnect();
}
