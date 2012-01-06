#pragma once
#include <Util/Base.h>
#include <Concurrency/Concurrency.h>

/* Test player class. */

namespace pwskoag
{
	class C_Player
	{
		protected:
			ushort		m_Id;
			std::string	m_Str;
			C_Mutex		m_Lock;
		public:
			virtual void M_SetId(ushort id)=0;
			virtual ushort M_Id() const=0;
			virtual void M_AddStr(std::string& str)=0;
			virtual void M_SetStr(std::string& str)=0;
			virtual std::string& M_GetStr()=0;
			virtual void M_Send()=0;
	};
}
