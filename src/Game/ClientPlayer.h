#pragma once
#include <Util/Base.h>
#include <Network/Network.h>
#include <Concurrency/Concurrency.h>
#include <Game/Player.h>
#include <Graphics/PlayerGfx.h>
#include <SFML/Graphics.hpp>

/* Test player class */

namespace pwskoag
{
	class TcpClient;
	class C_PlayerGfx;
	class C_ClientPlayer : public C_Player, public C_Sendable
	{
		private:
			ushort			m_Id;
			std::string		m_Str;
			C_Mutex			m_Lock;
			C_PlayerGfx*	m_Draw;
		public:
			C_ClientPlayer() : C_Sendable() { m_Draw = new C_PlayerGfx(); }
			C_ClientPlayer(TcpSocket *s, C_Packet* p) : C_Sendable(s,p) { m_Draw = new C_PlayerGfx(); }
			~C_ClientPlayer() { delete m_Draw; }
			C_PlayerGfx* M_GetDRAW() {return m_Draw;}
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
				m_Str+=str;
				m_Draw->M_SetStr(m_Str);
			}
			void M_SetStr(std::string& str)
			{
				C_Lock l(m_Lock);
				m_Str=str;
				m_Draw->M_SetStr(m_Str);
			}
			std::string& M_GetStr() {C_Lock l(m_Lock); return m_Str;}
			void M_Send()
			{
				C_Lock l(m_Lock);
				if(m_Packet && m_Tcp)
				{
					*m_Packet<<(uchar)Message<<(int)m_Tcp->M_Id()<<m_Str;
					m_Tcp->Send(*m_Packet);
				}
			}
	};
}
