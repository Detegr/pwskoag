#include <Util/Base.h>
#include "Packet.h"
namespace pwskoag
{
	const size_t C_Packet::MAXSIZE=4096;

	C_Packet::C_Packet(const C_Packet& rhs) : m_Data(rhs.M_Size()), m_Sections(rhs.M_Sections())
	{
		m_Lock=rhs.m_Lock;
		memcpy(&m_Data[0], &rhs.m_Data[0], rhs.M_Size());
		if(rhs.M_Sections()) memcpy(&m_Sections[0], &rhs.m_Sections[0], rhs.M_Sections()*sizeof(m_Sections[0]));
	}

	C_Packet& C_Packet::operator=(const C_Packet& rhs)
	{
		if(this!=&rhs)
		{
			this->m_Lock=rhs.m_Lock;
			C_Lock(this->m_Lock);
			m_Data.resize(rhs.M_Size());
			m_Sections.resize(rhs.M_Sections());
			memcpy(&m_Data[0], &rhs.m_Data[0], rhs.M_Size());
			if(M_Sections()) memcpy(&m_Sections[0], &rhs.m_Sections[0], rhs.M_Sections()*sizeof(m_Sections[0]));
		}
		return *this;
	}

	void C_Packet::M_Append(const void* d, size_t len)
	{
		C_Lock l(m_Lock);
		size_t size=m_Data.size();
		m_Sections.push_back(size);
		m_Data.resize(size+len);
		memcpy(&m_Data[size], d, len);
	}

	void C_Packet::M_Pop(size_t bytes)
	{
		C_Lock l(m_Lock);
		m_Sections.erase(m_Sections.begin());
		m_Data.erase(m_Data.begin(), m_Data.begin()+bytes);
	}
	uchar* C_Packet::M_GetSection(int section) const
	{
		if(section==m_Sections.size()) return (uchar*)&m_Data[m_Data.size()];
		else return (uchar*)&m_Data[m_Sections[section]];
	}

	void C_Packet::M_GetDataChunk(e_Command h, void* data)
	{
		switch(h)
		{
			case HandShake: return;
			case TCPConnect:
			{
				uint d;
				*this>>d;
				if(data) memcpy(data, &d, sizeof(d));
				return;
			}
			case UDPConnect:
			{
				ushort d;
				*this>>d;
				if(data) memcpy(data, &d, sizeof(d));
				return;
			}
			case String:
			{
				std::string d;
				*this>>d;
				if(data) memcpy(data, &d, sizeof(d));
				return;
			}
			case EOP: return;
			default: throw std::runtime_error("Unknown header type");
		}
	}

	C_Packet C_DeltaPacket::M_Delta(const C_Packet& rhs)
	{
		if(!m_Previous.M_Size()) return rhs;

		C_Packet out;
		int s=rhs.M_Sections();
		for(int i=0; i<s; ++i)
		{
			uchar* ps = m_Previous.M_GetSection(i);
			uchar* psnext = m_Previous.M_GetSection(i+1);
			uchar* rs = rhs.M_GetSection(i);
			uchar* rsnext = rhs.M_GetSection(i+1);

			if((psnext-ps) != (rsnext-rs)) out.M_Append(rs, (rsnext-rs));
			else
			{
				if(memcmp(ps, rs, (rsnext-rs))!=0)
				{
					if(rsnext-rs==1) out.M_Append(rs, 1);
					else
					{
						out.M_Append(rhs.M_GetSection(i-1), 1);
						out.M_Append(rs, rsnext-rs);
					}
				}
			}
		}
		m_Previous=rhs;
		return out;
	}
}
