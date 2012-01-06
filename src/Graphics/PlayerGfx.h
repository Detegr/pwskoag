#pragma once
#include <Graphics/Graphics.h>
namespace pwskoag
{
	class C_PlayerGfx : public C_Drawable
	{
		private:
			sf::Font	m_Font;
			sf::String	m_Text;
		public:
			C_PlayerGfx()
			{
				static uint y=0;
				m_Font.LoadFromFile("play.ttf");
				m_Text.SetFont(m_Font);
				m_Text.SetSize(20);
				m_Text.SetPosition(0,y);
				y+=20;
			}
			void M_SetStr(std::string& str)
			{
				m_Text.SetText(str);
			}
			const sf::Drawable* M_GetDrawableObj() const { return &m_Text; }
	};
}
