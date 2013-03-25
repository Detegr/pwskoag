#pragma once
#include <vector>
#include <iostream>
#include "model.h"

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
		bool Load(const std::string& name, const std::string& path);
		const C_Model& Get(const std::string& model) const;
		const C_Model* Create(const std::string& name, const std::vector<float>& verts, float width, float height);
};
