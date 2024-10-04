#pragma once

#include "Component.h"
#include <glm/glm.hpp> 

class Collider : public Component
{ 
	friend class CollisionManager;

public:
	
	virtual const glm::vec3 & GetCentre   ( ) const = 0;
	virtual const glm::quat & GetRotation ( ) const = 0;

	void Awake ( ) override;
	void OnDestroy ( ) override;

protected:

	Collider ( GameObject & hostObject, ComponentTypes type )
		: Component ( hostObject, type )
	{ }

};

