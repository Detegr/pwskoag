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
			Fixed,
			Bullet
		};
	protected:
		C_Entity() {}
		C_Entity(const C_Entity&) {}
		virtual ~C_Entity();

		unsigned short m_Id;
		C_Model m_Model;
		float	m_Scale;
		bool	m_Dynamic;
		b2Body* m_Body;
		int		m_Data[2];

		C_Entity(b2World& w, const C_Model& m, float scale=1.0f, bool dynamic=true, Type t=Default);
		C_Entity(unsigned short id, b2World& w, const C_Model& m, float s, bool dynamic, Type t=Default);
		void M_Initialize(unsigned short id, b2World& w, const C_Model& m, float s, bool dynamic, Type t);
	public:
		static const int BULLET_HITS=2;
		b2Body* M_Body() { return m_Body; }
		C_Vec2 GetPosition() const;
		void SetPosition(float x, float y);
		void SetPosition(const C_Vec2& v);
		float GetRotation() const;
		void SetRotation(float r);
		void operator>>(dtglib::C_Packet& p);
		void M_DumpFullInstance(dtglib::C_Packet& p);
		float M_Scale() const {return m_Scale;}
		std::string M_Name() const {return m_Model.M_Name();}
		unsigned short M_Id() const {return m_Id;}
};
