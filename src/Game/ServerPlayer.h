#pragma once
#include "Player.h"
#include <dtglib/Network.h>

namespace pwskoag
{	
	class C_ServerPlayer : public C_Entity, public C_PlayerBase
	{
		public:
			C_ServerPlayer(C_TcpSocket* s, C_Packet* p);
			~C_ServerPlayer();
			void M_Id(ushort id);
			ushort M_Id() const;
			void M_Send();
			void M_SendUdp(C_UdpSocket& s);
			void M_Position(const C_Vec2& v);
			const C_Vec2& M_Position() const;
	};
}
