#include "Timer.h"
namespace pwskoag
{
	PWSKOAG_API C_Timer::C_Timer() {M_Reset();}
	PWSKOAG_API uint64 C_Timer::M_Get() const
	{
		#ifdef _WIN32
			return (GetTickCount64()-m_Time);
		#else
			struct timeval cur;
			gettimeofday(&cur, NULL);
			return uint64(((cur.tv_sec*1000)+(cur.tv_usec/1000))-((m_Time.tv_sec*1000)+(m_Time.tv_usec/1000)));
		#endif
	}
	PWSKOAG_API void C_Timer::M_Reset()
	{
		#ifdef _WIN32
			m_Time=GetTickCount64();
		#else
			gettimeofday(&m_Time,NULL);
		#endif
	}
}
