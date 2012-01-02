#include <Util/Base.h>
#include "Packet.h"
namespace pwskoag
{
	const size_t C_Packet::MAXSIZE=4096;

	C_Packet::C_Packet(const C_Packet& rhs) : m_Data(rhs.M_Size())
	{
		m_Lock=rhs.m_Lock;
		memcpy(&m_Data[0], &rhs.m_Data[0], rhs.M_Size());
	}

	C_Packet& C_Packet::operator=(const C_Packet& rhs)
	{
		if(this!=&rhs)
		{
			this->m_Lock=rhs.m_Lock;
			C_Lock(this->m_Lock);
			m_Data.resize(rhs.M_Size());
			memcpy(&m_Data[0], &rhs.m_Data[0], rhs.M_Size());
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
	uchar* C_Packet::M_GetSection(int section)
	{
		if(section==m_Sections.size()) return &m_Data[m_Data.size()];
		else return &m_Data[m_Sections[section]];
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
				if(data) *reinterpret_cast<std::string*>(data)=d;
				return;
			}
			case EOP: return;
		}
	}

	C_Packet C_DeltaPacket::M_Delta(const C_Packet& rhs) const
	{
		if(!m_Previous.M_Size()) return rhs;

		C_Packet out;
		C_Packet pretmp(m_Previous);
		C_Packet rhstmp(rhs);

		uchar rhsheader;
		uchar preheader;
		while(rhstmp.M_Size())
		{
			rhstmp>>rhsheader;
			pretmp>>preheader;
			if(rhsheader==preheader)
			{
				switch(rhsheader)
				{
					case HandShake:
					{
						out<<rhsheader;
						break;
					}
					case TCPConnect:
					{
						uint predata;
						uint rhsdata;
						pretmp>>predata;
						rhstmp>>rhsdata;
						if(predata!=rhsdata)
						{
							uchar rhsheader2;
							uint rhsdata2;
							C_Packet rhstmp2(rhstmp);
							bool nonewdata=false;
							while(rhstmp2.M_Size())
							{
								rhstmp2>>rhsheader2;
								if(rhsheader2==preheader)
								{
									rhstmp2.M_GetDataChunk(TCPConnect, &rhsdata2);
									if(rhsdata2==predata)
									{
										nonewdata=true;
										break;
									}
								}
								else rhstmp2.M_GetDataChunk((e_Command)rhsheader2, NULL);
							}
							if(!nonewdata) out<<rhsheader<<rhsdata;
						}
						break;
					}
					case UDPConnect:
					{
						ushort predata;
						ushort rhsdata;
						pretmp>>predata;
						pretmp>>rhsdata;
						if(predata!=rhsdata) out<<rhsheader<<rhsdata;
						break;
					}
					case String:
					{
						std::string predata;
						std::string rhsdata;
						pretmp>>predata;
						rhstmp>>rhsdata;
						if(predata!=rhsdata) out<<rhsheader<<rhsdata;
						break;
					}
					case EOP:
					{
						out<<rhsheader;
						break;
					}
					default:
					{
						throw std::runtime_error("Wrong packet header.");
					}
				}
			}
		}
		return out;
	}
}
