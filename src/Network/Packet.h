#pragma once
#include <Concurrency/Concurrency.h>
#include <vector>
namespace pwskoag
{
	/*
	 * Packet variable types&structures:
	 * HandShake	[uchar(HandShake)]
	 * TCPConnect 	[uchar(TCPConnect), uint]
	 * UDPConnect 	[uchar(UDPConnect), ushort]
	 * String		[uchar(String), std::string]
	 * Message		[uchar(Message), int(Id), std::string]
	 * EOP			[uchar(EOP)]
	 */
	enum e_Command
	{
		HandShake=0,
		Heartbeat,
		TCPConnect,
		UDPConnect,
		Disconnect,
		ClientDisconnected,
		Message,
		String,
		MessageTimer,
		EOP=255
	};

	class C_Packet
	{
		friend class Socket;
		friend class C_DeltaPacket;
		private:
			C_Mutex				m_Lock;
			std::vector<uchar>	m_Data;
			std::vector<size_t> m_Sections;
			void 				M_Append(const void* d, size_t len);
			void				M_Pop(size_t bytes);
		public:
			static const size_t	MAXSIZE;
			C_Packet() {}
			C_Packet(const C_Packet& rhs);
			C_Packet& 			operator=(const C_Packet& rhs);
			const uchar* 		M_RawData() const {return &m_Data[0];}
			void 				M_Clear() {m_Data.clear();}
			size_t				M_Size() const {return m_Data.size();}
			size_t				M_Sections() const {return m_Sections.size();}
			C_Packet& operator<<(const char* str)
			{
				M_Append(str, strlen(str)+1);
				return *this;
			}
			C_Packet& operator<<(const std::string& str)
			{
				M_Append(str.c_str(), str.length()+1);
				return *this;
			}
			C_Packet& operator>>(char* str)
			{
				strcpy(str, (char*)&m_Data[0]);
				M_Pop(strlen(str)+1);
				return *this;
			}
			C_Packet& operator>>(std::string& str)
			{
				str=(char*)&m_Data[0];
				M_Pop(str.length()+1);
				return *this;
			}
			C_Packet& operator<<(e_Command c)
			{
				uchar x=(uchar)c;
				M_Append(&x, sizeof(x));
				return *this;
			}
			template <class type> C_Packet&	operator<<(type x)
			{
				M_Append(&x, sizeof(type));
				return *this;
			}
			template <class type> C_Packet&	operator>>(type& x)
			{
				if(M_Size())
				{
					x=*(type*)&m_Data[0];
					M_Pop(sizeof(type));
				}
				return *this;
			}
			void				M_GetDataChunk(uchar h, void* data) {M_GetDataChunk((e_Command)h, data);}
			void				M_GetDataChunk(e_Command h, void* data);
			uchar*				M_GetSection(int section) const;
	};

	class C_DeltaPacket
	{
		private:
			C_Packet m_Previous;
		public:
			C_DeltaPacket(const C_Packet& rhs) : m_Previous(rhs) {}
			C_Packet M_Delta(const C_Packet& rhs);
	};
}
