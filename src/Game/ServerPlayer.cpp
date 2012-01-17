#include "ServerPlayer.h"

namespace pwskoag
{
	C_ServerPlayer::C_ServerPlayer(TcpSocket* s, C_Packet* p) : C_Player(s,p) {}
	C_ServerPlayer::~C_ServerPlayer() {}
	void C_ServerPlayer::M_SetId(ushort id)
	{
		m_Id=id;
	}
	ushort C_ServerPlayer::M_Id() const
	{
		return m_Id;
	}
	void C_ServerPlayer::M_AddStr(std::string& str)
	{
		C_Lock l(m_Lock);
		this->m_Str+=str;
	}
	void C_ServerPlayer::M_SetStr(std::string& str)
	{
		C_Lock l(m_Lock);
		this->m_Str=str;
	}
	std::string& C_ServerPlayer::M_GetStr() { C_Lock l(m_Lock); return m_Str; }
	void C_ServerPlayer::M_Send()
	{
		C_Lock l(m_Lock);
		this->m_Tcp->Send(*m_Packet);
	}
	void C_ServerPlayer::M_SendUdp(UdpSocket& s)
	{
		C_Lock l(m_Lock);
		s.Send(*m_Packet, m_Tcp->GetIp(), m_Tcp->M_UdpPort());
	}
	void C_ServerPlayer::M_Position(C_Vec2& v)
	{
		this->m_Position=v;
	}
}