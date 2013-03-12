#include "timer.h"

C_Timer::C_Timer() {M_Reset();}
double C_Timer::M_Get() const
{
	#ifdef _WIN32
		return (GetTickCount64()-m_Time) / 1000.0;
	#else
		struct timeval cur;
		gettimeofday(&cur, NULL);
		return (((cur.tv_sec*1000000)+(cur.tv_usec))-((m_Time.tv_sec*1000000)+(m_Time.tv_usec))) / 1000000.0;
	#endif
}

double C_Timer::M_Now() const
{
	#ifdef _WIN32
		return (GetTickCount64()) / 1000.0;
	#else
		struct timeval cur;
		gettimeofday(&cur, NULL);
		return (((cur.tv_sec*1000000)+(cur.tv_usec));
}

void C_Timer::M_Reset()
{
	#ifdef _WIN32
		m_Time=GetTickCount64();
	#else
		gettimeofday(&m_Time,NULL);
	#endif
}
