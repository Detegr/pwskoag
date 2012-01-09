#pragma once
#include <Util/Base.h>

namespace pwskoag
{
	struct Keyboard
	{
		static const char RETURN=-1;
		static const char BACKSPACE=-2;
		static char M_GetChar(uint c);
	};
	
	char Keyboard::M_GetChar(uint c)
	{
		std::cout << (int)c << std::endl;
		switch(c)
		{
			#ifdef __APPLE__
				case 278: return RETURN;
				case 279: return BACKSPACE;
				case 277: return ' ';
				case 270: return '.';
				case 269: return ',';
				case 271: return '\'';
			#endif
			default: return (char)c;
		}
	}
}