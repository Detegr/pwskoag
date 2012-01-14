#pragma once
#include <Graphics/Drawable.h>
#include <sstream>

namespace pwskoag
{
	class C_PlayerGfx : public C_Drawable
	{
		private:
			sf::Font	m_Font;
			sf::String	m_Text;
			sf::String	m_Time;
		public:
			C_PlayerGfx()
			{
				static uint y=0;
				m_Font.LoadFromFile("orbitron.otf");
				m_Text.SetFont(m_Font);
				m_Text.SetSize(20);
				m_Text.SetPosition(0,y);

				m_Time.SetFont(m_Font);
				m_Time.SetSize(20);
				m_Time.SetPosition(560,y);
				
				y+=20;
			}
			void M_SetStr(std::string& str)
			{
				m_Text.SetText(str);
			}
			void M_SetTime(uint time)
			{
				std::stringstream ss;
				ss<<time;
				std::string timestr;
				ss>>timestr;
				m_Time.SetText(timestr);
			}
			void M_Draw(sf::RenderWindow& w) const
			{
				w.Draw(m_Text);
				w.Draw(m_Time);
			}
	};
}
