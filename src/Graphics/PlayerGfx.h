#pragma once
#include <Graphics/Drawable.h>
#include <sstream>

namespace pwskoag
{
	class C_PlayerGfx : public C_Drawable
	{
		private:
			sf::Font	m_Font;
			sf::Text	m_Text;
			sf::Text	m_Time;
		public:
			C_PlayerGfx()
			{
				#ifdef __APPLE__
					// Hack to prevent double-freeing pointers from sf::String
					// This is apple's fault.
					m_Text.SetString(" ");
					m_Time.SetString(" ");
				#endif
				static uint y=0;
				m_Font.LoadFromFile("orbitron.otf");
				m_Text.SetFont(m_Font);
				m_Text.SetCharacterSize(20);
				m_Text.SetPosition(0,y);

				m_Time.SetFont(m_Font);
				m_Time.SetCharacterSize(20);
				m_Time.SetPosition(560,y);
				m_Text.SetString("PLAYER");
				m_Time.SetString(y);
				
				y+=20;
			}
			void M_SetStr(std::string& str)
			{
				m_Text.SetString(str);
			}
			void M_SetTime(uint time)
			{
				std::stringstream ss;
				ss<<time;
				std::string timestr;
				ss>>timestr;
				m_Time.SetString(timestr);
			}
			void M_Draw(sf::RenderWindow& w) const
			{
				w.Draw(m_Text);
				w.Draw(m_Time);
			}
	};
}
