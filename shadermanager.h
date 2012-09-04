#pragma once
#include <vector>
#include <iostream>
#include <GL3/gl3w.h>
#include <GL/glfw.h>
#include <GL3/gl3.h>

class C_Shader
{
	private:
		std::string m_Name;
		GLuint m_ProgId;
	public:
		C_Shader() : m_Name(), m_ProgId(0) {}
		C_Shader(const std::string& name, GLuint progid) :
			m_Name(name), m_ProgId(progid) {}
		void M_Clear() { glDeleteShader(m_ProgId); m_ProgId=0; }
		const std::string& M_Name() const { return m_Name; }
		GLuint M_Id() const { return m_ProgId; }
};

class C_ShaderManager
{
	friend class C_Singleton;
	private:
		C_ShaderManager()
		{
			std::cout << "Initializing shadermanager..." << std::flush;
			std::cout << "OK!" << std::endl;
		}
		~C_ShaderManager()
		{
			std::cout << "Destroying shadermanager..." << std::flush;
			for(std::vector<C_Shader>::iterator it=m_Shaders.begin(); it!=m_Shaders.end(); ++it)
			{
				it->M_Clear();
			}
			std::cout << "OK!" << std::endl;
		}
		std::vector<C_Shader> m_Shaders;
		bool M_CheckResult(GLuint id, GLuint status);
	public:
		bool M_Load(const std::string& name);
		const C_Shader& M_Get(const std::string& name) const;
};
