#pragma once

#define VERSION 1000

class C_Version
{
	private:
		static const unsigned int m_Magic=1337;
		static const unsigned int m_Ver=VERSION;
	public:
		// xorring just for fun :D
		static const unsigned int M_Get() {return m_Ver^m_Magic;}
		static bool M_Equal(const unsigned int ver) {return (ver^m_Magic)==m_Ver;}
};

#undef VERSION
