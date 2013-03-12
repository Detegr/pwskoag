#include "singleton.h"
#include "entity.h"

C_GfxEntity::C_GfxEntity() : m_Id(0), m_IsPlayer(false), m_Model(), m_Scale(1.0f), m_ModelMatrix(glm::mat4(1.0f)) {}
C_GfxEntity::C_GfxEntity(unsigned short id, const C_Model& m, float scale) : m_Id(id), m_IsPlayer(false), m_Model(m), m_Scale(scale), m_ModelMatrix(glm::mat4(1.0f))
{
	M_Scale(scale);
	glGenBuffers(1, &m_Vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*m_Model.M_Vertices().size(), &m_Model.M_Vertices()[0], GL_STATIC_DRAW);
}

C_GfxEntity* C_GfxEntity::M_Create(unsigned short id, const C_Model& m, float scale)
{
	C_GfxEntity* e=new C_GfxEntity(id, m, scale);
	C_Renderer* r = C_Singleton::M_Renderer();
	r->M_AddEntity(e);
	return e;
}

void C_GfxEntity::M_Translate(float amount, unsigned char axis)
{
	bool x=(axis|0xFE) == 0xFF;
	bool y=(axis|0xFD) == 0xFF;
	float xamount = x?amount:0.0f;
	float yamount = y?amount:0.0f;
	m_TranslationMatrix = glm::translate(m_TranslationMatrix, glm::vec3(xamount,yamount,0.0f));
}

void C_GfxEntity::SetPosition(float x, float y)
{
	m_Ex.M_Add(x);
	m_Ey.M_Add(y);
	m_TranslationMatrix = glm::translate(glm::mat4(1.0), glm::vec3(x/m_Scale,y/m_Scale,0.0f));
}

void C_GfxEntity::SetPosition(const C_Vec2& v)
{
	m_TranslationMatrix = glm::translate(glm::mat4(1.0), glm::vec3(v.x/m_Scale, v.y/m_Scale, 0.0f));
}

const C_Vec2 C_GfxEntity::GetPosition() const
{
	return C_Vec2(m_TranslationMatrix[3][0]*m_Scale, m_TranslationMatrix[3][1]*m_Scale);
}

void C_GfxEntity::M_ExtrapolatePosition(double dt)
{
	float xx = m_Ex.M_ExtrapolateValue();
	float yy = m_Ey.M_ExtrapolateValue();
	float xxx = m_Ex.M_Current() + ((xx - m_Ex.M_Current()) * (dt/0.04));
	float yyy = m_Ey.M_Current() + ((yy - m_Ey.M_Current()) * (dt/0.04));
	m_TranslationMatrix = glm::translate(glm::mat4(1.0), glm::vec3(xxx/m_Scale,yyy/m_Scale,0.0f));
}

void C_GfxEntity::M_Scale(float amount)
{
	m_ScaleMatrix = glm::scale(m_ScaleMatrix, glm::vec3(amount));
}

void C_GfxEntity::M_SetScale(float amount)
{
	m_ScaleMatrix = glm::scale(glm::mat4(1.0), glm::vec3(amount));
}

void C_GfxEntity::M_Rotate(float amount)
{
	m_RotationMatrix = glm::rotate(m_RotationMatrix, amount, glm::vec3(0.0f, 0.0f, 1.0f));
}

void C_GfxEntity::M_SetRotation(float amount)
{
	m_Er.M_Add(amount*(180/3.14));
	m_RotationMatrix = glm::rotate(glm::mat4(1.0), (const float)(amount*(180/3.14)), glm::vec3(0.0f, 0.0f, 1.0f));
}
void C_GfxEntity::M_ExtrapolateRotation(double dt)
{
	float a=m_Er.M_ExtrapolateValue();
	float ca=m_Er.M_Current() + ((a - m_Er.M_Current()) * (dt/0.04));
	m_RotationMatrix = glm::rotate(glm::mat4(1.0), (const float)ca, glm::vec3(0.0f, 0.0f, 1.0f));
}
const glm::mat4& C_GfxEntity::M_ModelMatrix()
{
	m_ModelMatrix = m_ScaleMatrix * m_TranslationMatrix * m_RotationMatrix;
	return m_ModelMatrix;
}

const std::string& C_GfxEntity::M_ModelName() const
{
	return m_Model.M_Name();
}

void C_GfxEntity::M_Draw() const
{
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
	glVertexAttribPointer(0,C_Model::COMPONENTS_PER_VERT,GL_FLOAT,GL_FALSE,0,(void*)0);
	glDrawArrays(GL_TRIANGLE_STRIP,0,m_Model.M_Vertices().size());
	glDisableVertexAttribArray(0);
}
