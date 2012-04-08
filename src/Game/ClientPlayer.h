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
	struct C_GfxEntity : public C_Entity
	{
		C_GfxEntity() : C_Entity() {}
		C_GfxEntity(TcpSocket* s, C_Packet* p) : C_Entity(s,p) {}
		virtual void				M_SetId(ushort id)=0;
		virtual ushort				M_Id() const=0;
		virtual void				M_Position(C_Vec2& v)=0;
		virtual const C_Drawable&	M_GetDRAW() const=0;
	};

	class C_ClientPlayer : public C_PlayerBase, public C_GfxEntity
	{
		private:
			C_PlayerGfx		m_Draw;
		public:
			C_ClientPlayer();
			C_ClientPlayer(TcpSocket *s, C_Packet* p);
			const C_PlayerGfx& M_GetDRAW() const;
			void M_SetId(ushort id);
			ushort M_Id() const;
			void M_AddStr(std::string& str);
			void M_SetStr(std::string& str);
			std::string& M_GetStr();
			void M_Send();
			void M_SendUdp(UdpSocket& s);
			void M_Time(uint time);
			void M_Position(C_Vec2& v);
	};
}
