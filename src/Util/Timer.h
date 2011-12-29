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
			struct timeval m_Time;
		public:
			C_Timer();
			uint64 M_Get() const;
			void M_Reset();
	};
}
