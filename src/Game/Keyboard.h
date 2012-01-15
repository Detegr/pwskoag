#pragma once
#include <Util/Base.h>

namespace pwskoag
{
	struct Keyboard
	{
		#ifdef __APPLE__
			static const char RETURN=58;
			static const char BACKSPACE=59;
		#else
			static const char RETURN=13;
			static const char BACKSPACE=8;
		#endif
		static char M_GetChar(uint c);
	};
	
	char Keyboard::M_GetChar(uint c)
	{
		return (char)c;
	}
}
