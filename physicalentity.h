#pragma once
#include <Box2D/Box2D.h>
#include <Box2D/Common/b2Math.h>
#include "model.h"
#include "vec2.h"

class C_Entity
{
	friend class C_PhysicsManager;
	public:
		enum Type
		{
			Default=0,
			Bullet
		};
	protected:
		C_Entity() {}
		C_Entity(const C_Entity&) {}

		unsigned short m_Id;
		C_Model m_Model;
		float	m_Scale;
		bool	m_Dynamic;
		b2Body* m_Body;
		int		m_Data[2];

		C_Entity(b2World& w, const C_Model& m, float scale=1.0f, bool dynamic=true, Type t=Default);
	public:
		b2Body* M_Body() { return m_Body; }
		void M_SetPosition(float x, float y);
		void operator>>(dtglib::C_Packet& p);
		void M_DumpFullInstance(dtglib::C_Packet& p);
		float M_Scale() const {return m_Scale;}
		std::string M_Name() const {return m_Model.M_Name();}
		unsigned short M_Id() const {return m_Id;}
};
