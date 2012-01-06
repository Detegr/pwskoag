#pragma once
#include <Util/Base.h>
#include <Network/Network_common.h>
#include <Network/Network.h>
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

	class C_ServerPlayer : public C_Player, public C_Sendable
	{
		public:
			C_ServerPlayer(TcpSocket* s, C_Packet* p) : C_Sendable(s,p) {}
			~C_ServerPlayer() {}
			void M_SetId(ushort id)
			{
				m_Id=id;
			}
			ushort M_Id() const
			{
				return m_Id;
			}
			void M_AddStr(std::string& str)
			{
				C_Lock l(m_Lock);
				this->m_Str+=str;
			}
			void M_SetStr(std::string& str)
			{
				C_Lock l(m_Lock);
				this->m_Str=str;
			}
			std::string& M_GetStr() { C_Lock l(m_Lock); return m_Str; }
			void M_Send()
			{
				C_Lock l(m_Lock);
				this->m_Tcp->Send(*m_Packet);
			}
	};
}
