#include "shadermanager.h"
#include "filereader.h"
#include <stdexcept>
#include <string>

bool C_ShaderManager::M_CheckResult(GLuint id, GLuint status)
{
	GLint result = GL_FALSE;
	int infologlength=0;

	glGetShaderiv(id, status, &result);
	glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infologlength);
	if(infologlength>1)
	{
		std::string infolog(infologlength, ' ');
		glGetShaderInfoLog(id, infologlength, NULL, &infolog[0]);
		std::cout << "FAILED! Info log length: " << infologlength << " characters." << std::endl;
		std::cout << "==== INFOLOG ====" << std::endl;
		std::cout << infolog << std::endl;
		return false;
	}
	std::cout << "OK!" << std::endl;
	return true;
}

bool C_ShaderManager::M_Load(const std::string& name)
{
	GLuint vertid = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragid = glCreateShader(GL_FRAGMENT_SHADER);

	std::string vert = C_FileReader::M_ReadToStr(name+".vert");
	std::string frag = C_FileReader::M_ReadToStr(name+".frag");
	if(!(frag.length() || vert.length())) return false;

	std::cout << name << ".vert..." << std::flush;
	char const* vertsrc = vert.c_str();
	glShaderSource(vertid, 1, &vertsrc, NULL);
	glCompileShader(vertid);
	if(!M_CheckResult(vertid, GL_COMPILE_STATUS)) return false;

	std::cout << name << ".frag..." << std::flush;
	char const* fragsrc = frag.c_str();
	glShaderSource(fragid, 1, &fragsrc, NULL);
	glCompileShader(fragid);
	if(!M_CheckResult(fragid, GL_COMPILE_STATUS)) return false;

	std::cout << "Linking " << name << "..." << std::flush;
	GLuint pid = glCreateProgram();
	glAttachShader(pid, vertid);
	glAttachShader(pid, fragid);
	glLinkProgram(pid);
	if(!M_CheckResult(pid, GL_LINK_STATUS)) return false;

	glDeleteShader(vertid);
	glDeleteShader(fragid);

	m_Shaders.push_back(C_Shader(name,pid));

	return true;
}

const C_Shader& C_ShaderManager::M_Get(const std::string& name) const
{
	for(std::vector<C_Shader>::const_iterator it=m_Shaders.begin(); it!=m_Shaders.end(); ++it)
	{
		if(it->M_Name() == name) return *it;
	}
	throw std::runtime_error("Shader " + name + " does not exist!");
}
