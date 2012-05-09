#include "ServerPlayer.h"

namespace pwskoag
{
	C_ServerPlayer::C_ServerPlayer(C_TcpSocket* s, C_Packet* p) : C_Entity(s,p) {}
	C_ServerPlayer::~C_ServerPlayer() {}
	void C_ServerPlayer::M_Id(ushort id)
	{
		m_Id=id;
	}
	ushort C_ServerPlayer::M_Id() const
	{
		return m_Id;
	}
	void C_ServerPlayer::M_Send()
	{
		/*
		C_Lock l(m_Lock);
		this->m_Tcp->Send(*m_Packet);
		*/
	}
	void C_ServerPlayer::M_SendUdp(C_UdpSocket& s)
	{
		C_Lock l(m_Lock);
		s.M_Send(*m_Packet, m_Tcp->M_GetIp(), m_Tcp->M_UdpPort());
	}
	const C_Vec2& C_ServerPlayer::M_Position() const
	{
		return m_Position;
	}
	void C_ServerPlayer::M_Position(const C_Vec2& v)
	{
		this->m_Position=v;
	}
}
