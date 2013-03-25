#pragma once
#include <GL3/gl3w.h>
#include <GL/glfw.h>
#include <GL3/gl3.h>
#include "input.h"
#include <iostream>

class C_InputHandler
{
	friend class C_Singleton;
	private:

		C_InputHandler();
		~C_InputHandler()
		{
			std::cout << "Destroying inputhandler..." << std::flush;
			std::cout << "OK!" << std::endl;
		}
		static void M_HandleKey(int,int);

		static unsigned char m_KeysDown[12];
	public:
		static bool Get(Key k);
};

