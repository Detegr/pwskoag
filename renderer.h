#pragma once
#include <GL3/gl3w.h>
#include <GL/glfw.h>
#include <GL3/gl3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "shadermanager.h"
#include "modelmanager.h"

class C_Renderer
{
	friend class C_Singleton;
	private:

		public: float m_AspectRatio;
		glm::mat4 m_Projection;
		glm::mat4 m_View;
		GLuint m_VertexArray;

		GLuint m_CurrentShader;
		GLuint m_MVP;
		std::vector<C_GfxEntity*> m_Entities;

		C_Renderer(unsigned width, unsigned height);
		~C_Renderer();
	public:
		bool M_Opened() {return glfwGetWindowParam(GLFW_OPENED);}
		void M_Draw();
		void M_Use(const C_Shader& s);
		void M_AddEntity(C_GfxEntity* e);
		void M_DeleteEntity(C_GfxEntity* e);
		C_GfxEntity* M_GetEntity(unsigned short id);
		const std::vector<C_GfxEntity*>& M_Entities() const { return m_Entities; }
};
