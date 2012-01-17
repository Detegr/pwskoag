#pragma once
#include <Util/Base.h>

class C_Vec2
{
	private:
		uint	m_X;
		uint	m_Y;
	public:
		C_Vec2() : m_X(0), m_Y(0) {}
		C_Vec2(uint x, uint y) : m_X(x), m_Y(y) {}
		void	M_Position(uint x, uint y) {m_X=x; m_Y=y;}
		void	M_X(uint x) {m_X=x;}
		void	M_Y(uint y) {m_Y=y;}
		uint	M_X() const {return m_X;}
		uint	M_Y() const {return m_Y;}
};