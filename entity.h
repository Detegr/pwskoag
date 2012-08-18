#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "model.h"
#include "vec2.h"

class C_GfxEntity
{
	private:
		C_Vec2 m_Pos;
		C_Model m_Model;
		float m_Scale;

		GLuint m_Vbo;

		glm::mat4 m_ModelMatrix;
		glm::mat4 m_TranslationMatrix;
		glm::mat4 m_ScaleMatrix;
		glm::mat4 m_RotationMatrix;
		C_GfxEntity();
		C_GfxEntity(const C_Model& m, float scale);
	public:
		static C_GfxEntity* M_Create(const C_Model& m, float scale=1.0f);
		virtual void M_Translate(float amount, unsigned char axis);
		virtual void M_SetPosition(float x, float y);
		void M_Scale(float amount);
		float M_Scale() const { return m_Scale; }
		void M_SetScale(float amount);
		virtual void M_Rotate(float amount);
		virtual void M_SetRotation(float amount);
		const std::string& M_ModelName() const;
		virtual const glm::mat4& M_ModelMatrix();
		void M_Draw() const;
};
