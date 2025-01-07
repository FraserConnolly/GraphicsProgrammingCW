#pragma once

#include "GameObject.h"
#include "Time.h"

/// <summary>
/// The object this component is attached to will always look at the target transform.
/// </summary>
class LookAt : public Component
{
	friend class GameObject;

private:

	LookAt ( GameObject & hostObject );

	~LookAt ( )
	{ }

public:

	void Update ( ) override;

	void SetTarget ( const Transform * target );
	
	void Deserialise ( const json & data ) override;

private:

	Transform * m_target = nullptr;

};

