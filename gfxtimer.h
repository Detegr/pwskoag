#pragma once
#include <GL3/gl3w.h>
#include <GL/glfw.h>
#include <GL3/gl3.h>
#include <cstdio>
#include <cstring>

class C_GfxTimer
{
	private:
		unsigned int m_Frames;
		double m_Time;
		double m_PrevTime;
		double m_Dt;
		float m_MSPF;
		char title[64];
	public:
		C_GfxTimer();
		void M_SetTime();
		double M_DT() const;
		double M_MSPF() const;
};
