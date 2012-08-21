#include "renderer.h"
#include <iostream>
#include <stdexcept>
#include "glm/gtc/type_ptr.hpp"

C_Renderer::C_Renderer(unsigned width, unsigned height) :
	m_AspectRatio((float)width/(float)height),
	m_Projection(glm::perspective(90.0f, m_AspectRatio, 0.1f, 100.0f)),
	m_View(glm::lookAt(glm::vec3(0,0,1), glm::vec3(0,0,0), glm::vec3(0,1,0)))
{
	std::cout << "Initializing renderer..." << std::flush;
	if(!glfwInit()) throw std::runtime_error("Could not initialize renderer.");
	glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
	glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	if(!glfwOpenWindow(width,height,0,0,0,0,0,0,GLFW_WINDOW))
	{
		glfwTerminate();
		throw std::runtime_error("Could not open window.");
	}
	if(gl3wInit())
	{
		glfwTerminate();
		throw std::runtime_error("Could not initialize gl3w.");
	}
	if(!gl3wIsSupported(3,2))
	{
		glfwTerminate();
		throw std::runtime_error("OpenGL 3.3 is not supported.");
	}
	glClearColor(0.0f, 0.0f, 0.3f, 0.0f);

	glGenVertexArrays(1,&m_VertexArray);
	glBindVertexArray(m_VertexArray);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	std::cout << "OK!" << std::endl;
}

void C_Renderer::M_Use(const C_Shader& s)
{
	m_CurrentShader=s.M_Id();
	glUseProgram(m_CurrentShader);
	m_MVP = glGetUniformLocation(m_CurrentShader, "MVP");
}

void C_Renderer::M_Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for(std::vector<C_GfxEntity*>::iterator it=m_Entities.begin(); it!=m_Entities.end(); ++it)
	{
		glm::mat4 MVP=m_Projection*m_View*(*it)->M_ModelMatrix();
		glUniformMatrix4fv(m_MVP, 1, GL_FALSE, glm::value_ptr(MVP));
		(*it)->M_Draw();
	}
	glfwSwapBuffers();
}

C_Renderer::~C_Renderer()
{
	std::cout << "Destroying renderer..." << std::flush;
	for(std::vector<C_GfxEntity*>::iterator it=m_Entities.begin(); it!=m_Entities.end(); ++it)
	{
		delete *it;
	}
	glfwTerminate();
	std::cout << "OK!" << std::endl;
}

void C_Renderer::M_AddEntity(C_GfxEntity* e)
{
	m_Entities.push_back(e);
}
void C_Renderer::M_DeleteEntity(C_GfxEntity* e)
{
	for(std::vector<C_GfxEntity*>::iterator it=m_Entities.begin(); it!=m_Entities.end(); ++it)
	{
		if((*it) == e)
		{
			m_Entities.erase(it);
			delete e;
			return;
		}
	}
}

C_GfxEntity* C_Renderer::M_GetEntity(unsigned short id)
{
	for(std::vector<C_GfxEntity*>::iterator it=m_Entities.begin(); it!=m_Entities.end(); ++it)
	{
		if((*it)->M_Id() == id) return *it;
	}
	return NULL;
}
