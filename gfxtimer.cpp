#include "gfxtimer.h"

C_GfxTimer::C_GfxTimer() : m_Frames(0), m_Time(glfwGetTime())
{
	m_PrevTime=0;
	m_Dt=0;
	memset(title, 0, 64);
}

void C_GfxTimer::M_SetTime()
{
	m_Frames++;
	double t=glfwGetTime();
	m_Dt=t-m_PrevTime;
	m_PrevTime=t;
	if(1.0f + m_Time <= t)
	{
		m_MSPF=(t-m_Time)*1000 / m_Frames;
		sprintf(title, "ms per frame: %f", m_MSPF);
		glfwSetWindowTitle(title);
		m_Time=t;
		m_Frames=0;
	}
}
double C_GfxTimer::M_DT() const { return m_Dt; }
double C_GfxTimer::M_MSPF() const { return m_MSPF; }
