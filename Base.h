/* Base header for pwskoag.
 * Includes basic typedefs etc.
 */

#pragma once

typedef unsigned int 	uint;
typedef unsigned short 	ushort;
typedef unsigned char 	uchar;

inline void msSleep(uint ms)
{
	#ifdef _WIN32
		Sleep(ms);
	#else
		usleep(ms*1000);
	#endif
}
