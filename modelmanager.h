#pragma once
#include <GL3/gl3w.h>
#include <GL/glfw.h>
#include <GL3/gl3.h>
#include <vector>
#include <iostream>
#include "model.h"
#include "entity.h"

class C_ModelManager
{
	friend class C_Singleton;
	friend class C_GfxManager;
	public: static const unsigned short COMPONENTS_PER_VERT=2; // Change this to 3 if using 3D models.
	private:
		C_ModelManager() { std::cout << "Initializing modelmanager...OK!" << std::endl; }
		~C_ModelManager();
		std::vector<C_Model*> m_Models;
	public:
		bool M_Load(const std::string& name, const std::string& path);
		C_Model* M_Get(const std::string& model) const;
};
