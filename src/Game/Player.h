#pragma once
#include <Util/Base.h>
#include <Concurrency/Concurrency.h>
#include <Network/Network.h>

/* Test player class. */

namespace pwskoag
{
	class C_Entity
	{
		protected:
			ushort			m_Id;
			C_Mutex			m_Lock;
		public:
			virtual void	M_SetId(ushort id)=0;
			virtual ushort	M_Id() const=0;
	};
	
	struct C_SendableEntity : public C_Entity, public C_Sendable
	{
		C_SendableEntity() : C_Sendable() {}
		C_SendableEntity(TcpSocket* s, C_Packet* p) : C_Sendable(s,p) {}	
	};
	
	class C_Player : public C_SendableEntity
	{
		protected:
			std::string	m_Str;
		public:
			C_Player() : C_SendableEntity() {}
			C_Player(TcpSocket* s, C_Packet* p) : C_SendableEntity(s,p) {}
			virtual void M_AddStr(std::string& str)=0;
			virtual void M_SetStr(std::string& str)=0;
			virtual std::string& M_GetStr()=0;
			virtual void M_Send()=0;
	};
}
