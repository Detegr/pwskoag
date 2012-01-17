#pragma once
#include "Player.h"
#include <Network/Network.h>

namespace pwskoag
{	
	class C_ServerPlayer : public C_Player
	{
		public:
			C_ServerPlayer(TcpSocket* s, C_Packet* p);
			~C_ServerPlayer();
			void M_SetId(ushort id);
			ushort M_Id() const;
			void M_AddStr(std::string& str);
			void M_SetStr(std::string& str);
			std::string& M_GetStr();
			void M_Send();
			void M_SendUdp(UdpSocket& s);
			void M_Position(C_Vec2& v);
	};
}
