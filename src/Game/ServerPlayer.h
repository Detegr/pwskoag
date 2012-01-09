#pragma once
#include "Player.h"
#include "Network.h"

namespace pwskoag
{	
	class C_ServerPlayer : public C_Player//, public C_Sendable
	{
		public:
			C_ServerPlayer(TcpSocket* s, C_Packet* p) : C_Player(s,p) {}
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