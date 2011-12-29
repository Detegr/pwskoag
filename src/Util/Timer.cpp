#include "Timer.h"
namespace pwskoag
{
	C_Timer::C_Timer() {M_Reset();}
	uint64 C_Timer::M_Get() const
	{
		struct timeval cur;
		gettimeofday(&cur, NULL);
		return uint64(((cur.tv_sec*1000)+(cur.tv_usec/1000))-((m_Time.tv_sec*1000)+(m_Time.tv_usec/1000)));
	}
	inline void C_Timer::M_Reset() {gettimeofday(&m_Time,NULL);}
}
