#pragma once
#include <Util/Base.h>
#include <Network/Network_common.h>
#include <Network/Network.h>
#include <Concurrency/Concurrency.h>
#include <SFML/Graphics.hpp>

/* Test player class. */

namespace pwskoag
{
	struct C_Drawable
	{
		virtual ~C_Drawable() {}
		virtual const sf::Drawable* M_GetDrawableObj() const=0;
	};

	class C_DRAWPlayer : public C_Drawable
	{
		private:
			sf::Font	m_Font;
			sf::String	m_Text;
		public:
			C_DRAWPlayer()
			{
				static uint y=0;
				m_Font.LoadFromFile("play.ttf");
				m_Text.SetFont(m_Font);
				m_Text.SetSize(20);
				m_Text.SetPosition(0,y);
				y+=20;
			}
			void M_SetStr(std::string& str)
			{
				m_Text.SetText(str);
			}
			const sf::Drawable* M_GetDrawableObj() const { return &m_Text; }
	};

	class C_ServerPlayer : public C_Sendable
	{
		private:
			ushort			m_Id;
			std::string		m_Str;
			C_Mutex			m_Lock;
		public:
			C_ServerPlayer(TcpSocket* s, C_Packet* p) : C_Sendable(s,p) {}
			~C_ServerPlayer() {}
			void M_SetId(ushort id)
			{
				m_Id=id;
			}
			ushort M_Id()
			{
				return m_Id;
			}
			void M_AddStr(std::string& str)
			{
				C_Lock l(m_Lock);
				m_Str+=str;
			}
			void M_SetStr(std::string& str)
			{
				C_Lock l(m_Lock);
				m_Str=str;
			}
			std::string& M_GetStr() { C_Lock l(m_Lock); return m_Str; }
			void M_Send()
			{
				C_Lock l(m_Lock);
				m_Tcp->Send(*m_Packet);
			}
	};

	class TcpClient;
	class C_ClientPlayer : public C_Sendable
	{
		private:
			ushort			m_Id;
			std::string		m_Str;
			C_Mutex			m_Lock;
			C_DRAWPlayer*	m_Draw;
		public:
			C_ClientPlayer(TcpClient* t) : C_Sendable(t) { m_Draw = new C_DRAWPlayer(); }
			~C_ClientPlayer() { delete m_Draw; }
			C_DRAWPlayer* M_GetDRAW() {return m_Draw;}
			void M_SetId(ushort id)
			{
				m_Id=id;
			}
			ushort M_Id()
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
			std::string& M_GetStr() {return m_Str;}
			void M_Send()
			{
				C_Lock l(m_Lock);
				*m_Packet<<(uchar)Integer<<(int)m_Tcp->M_Id();
				*m_Packet<<(uchar)String<<m_Str;
				m_Tcp->Send(*m_Packet);
			}
	};

}
