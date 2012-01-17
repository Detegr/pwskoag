#pragma once
#include <Util/Base.h>
#include <Util/Timer.h>
#include <Util/Vec2.h>
#include <Concurrency/Concurrency.h>
#include <Network/Network.h>

namespace pwskoag
{
	class C_Entity
	{
		protected:
			C_Vec2			m_Position;
		public:
			virtual void	M_Position(C_Vec2& v)=0;
	};

	class C_NetEntity : public C_Sendable
	{
		protected:
			C_Mutex			m_Lock;
			ushort			m_Id;
		public:
			C_NetEntity() : C_Sendable() {}
			C_NetEntity(TcpSocket* s, C_Packet* p) : C_Sendable(s,p) {}
			void	M_Id(ushort id) {m_Id=id;};
			ushort	M_Id() const {return m_Id;}
	};

	class C_PlayerBase
	{
		protected:
			std::string m_Str;
		public:
			C_Timer		m_Time;
			virtual void M_AddStr(std::string& str)=0;
			virtual void M_SetStr(std::string& str)=0;
	};
}
