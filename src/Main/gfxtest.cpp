#include <Graphics/Graphics.h>
#include <Network/Network.h>

int main()
{
	pwskoag::C_Renderer r(640,480);
	pwskoag::TcpClient c;
	sf::Font f;
	f.LoadFromFile("play.ttf");
	sf::String text("", f, 40);
	r.M_AddObject(text);
	bool key=false;
	c.M_Connect("localhost", 55555);
	while(r.M_Running())
	{
		r.M_Draw();
		switch(r.M_GetEvent().Type)
		{
			case sf::Event::Closed: r.M_Stop(); break;
			case sf::Event::TextEntered:
			{
				sf::Uint32 e=r.M_GetEvent().Text.Unicode;
				if(e==278) text.SetText("");
				else
				{
					if(e==277) e=' ';
					std::cout << e << std::endl;
					std::string str(text.GetText());
					str+=(char)e;
					text.SetText(str);
				}
			}
			case sf::Event::KeyReleased: key=false; break;
		}
		pwskoag::msSleep(1000/30);
	}
	c.M_Disconnect();
}
