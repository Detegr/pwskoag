#include "ClientPlayer.h"
namespace pwskoag
{
	C_ClientPlayer::C_ClientPlayer() : C_GfxEntity() {}
	C_ClientPlayer::C_ClientPlayer(TcpSocket *s, C_Packet* p) : C_GfxEntity(s,p) {}
	const C_Drawable& C_ClientPlayer::M_GetDRAW() const {return m_Draw;}
	void C_ClientPlayer::M_SetId(ushort id)
	{
		this->m_Id=id;
	}
	ushort C_ClientPlayer::M_Id() const
	{
		return this->m_Id;
	}
	void C_ClientPlayer::M_Send()
	{
		C_Lock l(this->m_Lock);
		if(m_Packet && m_Tcp)
		{
			m_Tcp->Send(*m_Packet);
		}
	}
	void C_ClientPlayer::M_SendUdp(UdpSocket& s)
	{
		C_Lock l(this->m_Lock);
		if(m_Packet) 
	}
	void C_ClientPlayer::M_Time(uint time) {m_Draw.M_SetTime(time);}
	void C_ClientPlayer::M_Position(const C_Vec2& v) {m_Position=v;}
	const C_Vec2& C_ClientPlayer::M_Position() const { return m_Position; }
}
