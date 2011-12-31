#pragma once
#include <Concurrency/Concurrency.h>
#include <vector>
namespace pwskoag
{
	/*
	 * Packet types:
	 * TCPConnect [uchar(TCPConnect), uint]
	 * UDPConnect [uchar(UDPConnect), ushort]
	 */
	enum e_Command
	{
		HandShake=0,
		Heartbeat,
		TCPConnect,
		UDPConnect,
		Disconnect,
		String,
		EOP=255
	};

	class C_Packet
	{
		friend class Socket;
		private:
			C_Mutex				m_Lock;
			std::vector<uchar>	m_Data;
			void 				M_Append(const void* d, size_t len) {C_Lock l(m_Lock);m_Data.resize(m_Data.size()+len);memcpy(&m_Data[m_Data.size()-len], d, len);}
			void				Pop(size_t bytes) {C_Lock l(m_Lock);m_Data.erase(m_Data.begin(), m_Data.begin()+bytes);}
		public:
			static const size_t	MAXSIZE;
			const uchar* 		M_RawData() const {return &m_Data[0];}
			void 				M_Clear() {m_Data.clear();}
			size_t				M_Size() const {return m_Data.size();}
			C_Packet&			operator<<(const char* str) {M_Append(str, strlen(str)+1);}
			C_Packet&			operator<<(const std::string& str){M_Append(str.c_str(), str.length()+1); return *this;}
			C_Packet&			operator>>(char* str) {strcpy(str, (char*)&m_Data[0]); Pop(strlen(str)+1); return *this;}
			C_Packet&			operator>>(std::string& str) {str=(char*)&m_Data[0]; Pop(str.length()+1); return *this;}
			C_Packet&			operator<<(e_Command c) {uchar x=(uchar)c; M_Append(&x, sizeof(x)); return *this;}
			template <class type> C_Packet&		operator<<(type x) {M_Append(&x, sizeof(type)); return *this;}
			template <class type> C_Packet&		operator>>(type& x) {if(M_Size()){x=*(type*)&m_Data[0]; Pop(sizeof(type));} return *this;}
	};

	class C_DeltaPacket
	{
		private:
			C_Packet m_Previous;
		public:
			C_DeltaPacket(const C_Packet& rhs) : m_Previous(rhs) {}
			C_Packet M_Delta(const C_Packet& rhs) const;
	};
}
