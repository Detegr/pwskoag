#pragma once
#include <Util/Base.h>
#include <dtglib/Network.h>
#include <dtglib/Concurrency.h>
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
		virtual const C_Drawable& M_GetDRAW() const=0;
	};

	class C_ClientPlayer : public C_PlayerBase, public C_GfxEntity
	{
		private:
			C_PlayerGfx		m_Draw;
		public:
			C_ClientPlayer();
			C_ClientPlayer(TcpSocket *s, C_Packet* p);
			void M_SetId(ushort id);
			ushort M_Id() const;
			void M_Send();
			void M_SendUdp(UdpSocket& s);
			void M_Time(uint time);
			void M_Position(const C_Vec2& v);
			const C_Vec2& M_Position() const;
			const C_Drawable& M_GetDRAW() const;
	};
}
