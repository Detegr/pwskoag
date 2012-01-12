#pragma once
#include <Util/Base.h>

namespace pwskoag
{
	struct Keyboard
	{
		static const char RETURN=13;
		#ifdef _APPLE_
			static const char BACKSPACE=127;
		#else
			static const char BACKSPACE=8;
		#endif
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
