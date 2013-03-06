#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_access.hpp"
#include "model.h"
#include "vec2.h"
#include "ringbuffer.h"

class C_GfxEntity
{
	private:
		unsigned short m_Id;
		bool m_IsPlayer;
		C_Extrapolator<float> m_Ex;
		C_Extrapolator<float> m_Ey;
		//C_Vec2 m_Pos;
		C_Model m_Model;
		float m_Scale;
		C_Extrapolator<float> m_Er;

		GLuint m_Vbo;

		glm::mat4 m_ModelMatrix;
		glm::mat4 m_TranslationMatrix;
		glm::mat4 m_ScaleMatrix;
		glm::mat4 m_RotationMatrix;
		C_GfxEntity();
		C_GfxEntity(unsigned short id, const C_Model& m, float scale);
	public:
		static C_GfxEntity* M_Create(unsigned short id, const C_Model& m, float scale=1.0f);
		void M_Translate(float amount, unsigned char axis);
		void M_SetPosition(float x, float y);
		const C_Vec2 GetPosition() const;
		void M_ExtrapolatePosition(double dt);
		void M_Scale(float amount);
		float M_Scale() const { return m_Scale; }
		void M_SetScale(float amount);
		void M_Rotate(float amount);
		void M_SetRotation(float amount);
		void M_ExtrapolateRotation(double dt);
		const std::string& M_ModelName() const;
		const glm::mat4& M_ModelMatrix();
		void M_Draw() const;
		unsigned int M_Id() const { return m_Id; }
		bool IsPlayer() const { return m_IsPlayer; }
		void SetPlayer(bool b) { m_IsPlayer=b; }
};
