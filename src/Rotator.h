#pragma once
#include "Component.h"
#include <glm/glm.hpp>

class Rotator : public Component
{ 
	friend class GameObject;

private:

	Rotator( GameObject & hostObject ) : 
		Component( hostObject, ComponentTypes::ROTATOR ) { }
	~Rotator() { }
	
public:

	void Update ( );
	
	void SetSpeed ( float speed )
	{
		m_speed = speed;
		SetRotationAxis ( m_axisRotation );
	}

	float GetSpeed ( ) const
	{
		return m_speed;
	}

	void SetRotationAxis ( const bool x, const bool y, const bool z );

	void SetRotationAxis ( glm::bvec3 & axisRotation );

	const glm::bvec3 & GetRotationAxis ( ) const
	{
		return m_axisRotation;
	}

	void Deserialise ( const json & data );

private:

	float m_speed = 1.0f;
	float m_x_speed = 0.0f;
	float m_y_speed = 0.0f;
	float m_z_speed = 0.0f;

	glm::bvec3 m_axisRotation;

};

