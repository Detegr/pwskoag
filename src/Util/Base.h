/* Base header for pwskoag.
 * Includes basic typedefs etc.
 */
#pragma once

#ifdef _WIN32
	#include <WinSock2.h>
	#include <WS2tcpip.h>
	#include <Windows.h>
	#include <string>
	#define PWSKOAG_API __declspec( dllexport )
#else
	#include <arpa/inet.h>
	#include <errno.h>
	#define PWSKOAG_API __attribute__((visibility("default")))
#endif
#include <iostream>
#include <cstring>
#include <stdexcept>

namespace pwskoag
{
	#ifndef HAVE_INTTYPES_H
		typedef unsigned int		uint;
		typedef unsigned short		ushort;
		typedef unsigned char 		uchar;
		typedef unsigned long long	uint64;
	#endif

	PWSKOAG_API inline void msSleep(uint ms)
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

	PWSKOAG_API inline std::string Error(const char* err, int type=-1)
	{
		std::string error;
		error+=err;
		error+=": ";
		error+=strerror(errno);
		error+=" (Type: ";
		error+=type==SOCK_STREAM?"TCP)":"UDP)";
		return error;
	}
}
