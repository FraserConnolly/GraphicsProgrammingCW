#pragma once

#include <GL\glew.h>
#include "GameObject.h"

class Material;

class NoiseController : public Component
{
	friend class GameObject;

private:
	
	NoiseController ( GameObject & hostObject );

	~NoiseController ( )
	{ }

public:

	void Update ( ) override;
	
	void SetSpeed ( const float speed );
	const float GetSpeed ( ) const;
	
	void SetMaterial ( Material * const material );

	void Deserialise ( const json & data ) override;
	
private:

	float m_speed;
	Material * m_material;
	GLint m_uniformTimeLocation;
	GLint m_unifromFogDensity;
	GLint m_unifromFogColour;
	GLint m_uniformMaxDist;
	GLint m_UnifromMinDist;

};

