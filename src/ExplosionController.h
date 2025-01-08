#pragma once

#include <GL\glew.h>
#include "GameObject.h"

class Material;

class ExplosionController : public Component
{
	friend class GameObject;

private:
	
	ExplosionController ( GameObject & hostObject );

	~ExplosionController ( )
	{ }

public:

	void Update ( ) override;
	
	void SetSpeed ( const float speed ) { m_speed = speed; }
	const float GetSpeed ( ) const { return m_speed; }

	void SetExplosionAmount ( const float amount );
	
	void SetMaterial ( Material * const material );

	void Deserialise ( const json & data ) override;
	
private:

	float m_speed;
	Material * m_material;
	GLint m_uniformTimeLocation;

};

