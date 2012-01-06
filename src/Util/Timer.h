#pragma once

#include "Base.h"
#ifdef _WIN32
#else
	#include <sys/time.h>
#endif

namespace pwskoag
{
	class C_Timer
	{
		private:
			#ifdef _WIN32
				uint64 m_Time;
			#else
				struct timeval m_Time;
			#endif
		public:
			PWSKOAG_API C_Timer();
			PWSKOAG_API uint64 M_Get() const;
			PWSKOAG_API inline void M_Reset();
	};
}
