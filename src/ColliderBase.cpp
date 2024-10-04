#include "ColliderBase.h"
#include "CollisionManager.h"


void Collider::Awake ( )
{ 
	CollisionManager::RegisterCollider ( this );
}

void Collider::OnDestroy ( )
{ 
	CollisionManager::DeregisterCollider ( this );
}
