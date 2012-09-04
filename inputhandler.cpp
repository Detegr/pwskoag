#include "inputhandler.h"
#include <iostream>

unsigned char C_InputHandler::m_KeysDown[12];

void C_InputHandler::M_HandleKey(int key, int state)
{
	Key k;
	switch(key)
	{
		case GLFW_KEY_UP: k=UP; break;
		case GLFW_KEY_DOWN: k=DOWN; break;
		case GLFW_KEY_LEFT: k=LEFT; break;
		case GLFW_KEY_RIGHT: k=RIGHT; break;
		case GLFW_KEY_SPACE: k=SPACE; break;
		case GLFW_KEY_ESC: k=ESC; break;
		default: k=(Key)-1;
	}
	if(k!=-1) m_KeysDown[k]=state==GLFW_PRESS?true:false;
}

bool C_InputHandler::M_Get(Key k)
{
	return m_KeysDown[k];
}

C_InputHandler::C_InputHandler()
{
	std::cout << "Initializing inputhandler..." << std::flush;
	glfwSetKeyCallback(M_HandleKey);
	std::cout << "OK!" << std::endl;
}
