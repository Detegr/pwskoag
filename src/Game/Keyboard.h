#pragma once
#include <Util/Base.h>

namespace pwskoag
{
	struct Keyboard
	{
		static const char RETURN=13;
		static const char BACKSPACE=127;
		static char M_GetChar(uint c);
	};
	
	char Keyboard::M_GetChar(uint c)
	{
		switch(c)
		{
			default: return (char)c;
		}
	}
}