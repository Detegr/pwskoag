#pragma once
#include <SFML/Graphics.hpp>

struct C_Drawable
{
	virtual ~C_Drawable() {}
	virtual const sf::Drawable& M_GetDrawableObj() const=0;
};