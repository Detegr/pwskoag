#pragma once
#include <GL3/gl3w.h>
#include <GL/glfw.h>
#include <GL3/gl3.h>
#include <cstdio>
#include <cstring>
#include "dtglib/Base.h"

#ifdef _WIN32
#else
	#include <sys/time.h>
#endif

class C_Timer
{
	private:
		#ifdef _WIN32
			dtglib::uint64 m_Time;
		#else
			struct timeval m_Time;
		#endif
		C_Timer(const C_Timer&) {}
	public:
		C_Timer();
		double M_Get() const;
		void M_Reset();
};
