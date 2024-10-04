#pragma once
#include "ColliderBase.h"

class ColliderSphere :
    public Collider
{

    friend class GameObject;

    ColliderSphere ( GameObject & hostObject );

public:

    const glm::vec3 & GetCentre ( ) const override;
    const glm::quat & GetRotation ( ) const override;
    
    const float GetRadious ( ) const;
    void SetRadious ( const float radious );

private:

    float m_radious;

};

