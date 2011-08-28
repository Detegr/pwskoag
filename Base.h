/* Base header for pwskoag.
 * Includes basic typedefs etc.
 */
#include <arpa/inet.h>

#pragma once

typedef unsigned int 	uint;
typedef unsigned short 	ushort;
typedef unsigned char 	uchar;

inline void msSleep(uint ms)
{
	#ifdef _WIN32
		Sleep(ms);
	#else
		struct timespec ts;
		if(ms==0){sched_yield(); return;}
		else
		{
			ts.tv_sec=ms/1000;
			ts.tv_nsec=ms%1000*1000000;
		}
		nanosleep(&ts, NULL);
	#endif
}

inline std::string Error(const char* err, int type=-1)
{
	std::string error;
	error+=err;
	error+=": ";
	error+=strerror(errno);
	error+=" (Type: ";
	error+=type==SOCK_STREAM?"TCP)":"UDP)";
	return error;
}
