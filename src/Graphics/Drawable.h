#pragma once
#include <SFML/Graphics.hpp>

namespace pwskoag
{
	struct C_Drawable
	{
		virtual ~C_Drawable() {}
		virtual void M_Draw(sf::RenderWindow& w) const=0;
	};
}