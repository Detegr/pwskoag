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
		if(c.M_Connect("80.221.48.241", 55555))
		{
			p=c.M_OwnPlayer();
			u.M_Initialize(&c);
			u.M_Connect("80.221.48.241", 55556);
		}
		else throw std::runtime_error("fuu");
	}
	catch(...)
	{
		p=&fallbackp;
		std::cout << "Failed to connect..." << std::endl;
	}
	r.M_AddObject(*p);
	char prev;
	pwskoag::C_Timer chatclock;
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
			if(r.M_GetEvent().Type==sf::Event::Closed) break;
			else if(r.M_GetEvent().Type==sf::Event::TextEntered)
			{
				char c=pwskoag::Keyboard::M_GetChar(r.M_GetEvent().Text.Unicode);
				if(!(prev==c && chatclock.M_Get()<200))
				{
					std::string str;
					str+=c;
					p->M_AddStr(str);
					prev=c;
					chatclock.M_Reset();
				}
			}
			else if(r.M_GetEvent().Type==sf::Event::KeyPressed)
			{
				char c=pwskoag::Keyboard::M_GetChar(r.M_GetEvent().Key.Code);
				if(c==pwskoag::Keyboard::RETURN) {p->M_Send();}
				else if(c==pwskoag::Keyboard::BACKSPACE) {std::string str=p->M_GetStr(); if(str.size()) str.resize(str.size()-1); p->M_SetStr(str);}
			}
			pwskoag::msSleep(1);
		}
	}
	u.M_Disconnect();
	c.M_Disconnect();
}
