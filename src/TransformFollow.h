#pragma once

#include "GameObject.h"
#include "Time.h"

/// <summary>
/// The object this component is attached to will always look at the target transform.
/// </summary>
class TransformFollow : public Component
{
	friend class GameObject;

private:

	TransformFollow ( GameObject & hostObject );

	~TransformFollow ( )
	{ }

public:

	void Update ( ) override;

	void SetTarget ( const Transform * target ) { m_target = target; }
	void SetOffset ( const glm::vec3 & offset ) { m_offset = offset; }
	void SetRotationOffset ( const glm::vec3 & offset ) { m_rotationOffset = offset; }

	void Deserialise ( const json & data ) override;

private:

	const Transform * m_target = nullptr;
	glm::vec3 m_offset = glm::vec3 ( 0.0f );
	glm::vec3 m_rotationOffset = glm::vec3 ( 0.0f );

};

