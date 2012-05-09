#pragma once
#include <Util/Base.h>
#include <Util/Timer.h>
#include <Util/Vec2.h>
#include <dtglib/Concurrency.h>
#include <dtglib/Network.h>
#include <Network/Sendable.h>

using namespace dtglib;

namespace pwskoag
{
	class C_Entity : public C_Sendable
	{
		protected:
			C_Vec2			m_Position;
			C_Mutex			m_Lock;
			ushort			m_Id;
		public:
			C_Entity() : C_Sendable() {}
			C_Entity(C_TcpSocket* s, C_Packet* p) : C_Sendable(s,p) {}
			void	M_Id(ushort id) {m_Id=id;};
			ushort	M_Id() const {return m_Id;}
			virtual void M_Position(const C_Vec2& p)=0;
			virtual const C_Vec2& M_Position() const=0;
	};

	class C_PlayerBase
	{
		protected:
		public:
	};
}
