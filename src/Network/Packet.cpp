#include <Util/Base.h>
#include "Packet.h"
namespace pwskoag
{
	const size_t C_Packet::MAXSIZE=4096;
	C_Packet C_DeltaPacket::M_Delta(const C_Packet& rhs) const
	{
		if(!m_Previous.M_Size()) return rhs;

		C_Packet out;
		C_Packet pretmp(m_Previous);
		C_Packet rhstmp(rhs);

		uchar rhs_header;
		uchar pre_header;
		while(rhstmp.M_Size())
		{
			rhstmp>>rhsheader;
			pretmp>>preheader;
			if(rhsheader==preheader)
			{
				switch(rhsheader)
				{
					case HandShake: break;
					case TCPConnect:
					{
						uint predata;
						uint rhsdata;
						pretmp>>predata;
						rhstmp>>rhsdata;
						if(predata!=rhsdata) out<<rhsheader<<rhsdata;
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
					case EOP: break;
				}
			}
			else
			{
				out << rhsheader;
			}
		}
	}
}
