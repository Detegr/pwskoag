#include "ClientPlayer.h"
namespace pwskoag
{
	C_ClientPlayer::C_ClientPlayer() : C_Player() {}
	C_ClientPlayer::C_ClientPlayer(TcpSocket *s, C_Packet* p) : C_Player(s,p) {}
	const C_PlayerGfx& C_ClientPlayer::M_GetDRAW() const {return m_Draw;}
	void C_ClientPlayer::M_SetId(ushort id)
	{
		this->m_Id=id;
	}
	ushort C_ClientPlayer::M_Id() const
	{
		return this->m_Id;
	}
	void C_ClientPlayer::M_AddStr(std::string& str)
	{
		C_Lock l(this->m_Lock);
		m_Str+=str;
		m_Draw.M_SetStr(m_Str);
	}
	void C_ClientPlayer::M_SetStr(std::string& str)
	{
		C_Lock l(this->m_Lock);
		m_Str=str;
		m_Draw.M_SetStr(m_Str);
	}
	std::string& C_ClientPlayer::M_GetStr() {C_Lock l(m_Lock); return m_Str;}
	void C_ClientPlayer::M_Send()
	{
		C_Lock l(this->m_Lock);
		if(m_Packet && m_Tcp)
		{
			*m_Packet<<(uchar)Message<<(int)m_Tcp->M_Id()<<m_Str;
			m_Tcp->Send(*m_Packet);
		}
	}
	void C_ClientPlayer::M_SendUdp(UdpSocket& s) {}
	void C_ClientPlayer::M_Time(uint time) {m_Draw.M_SetTime(time);}
	void C_ClientPlayer::M_Position(C_Vec2& v) {m_Position=v;}
}