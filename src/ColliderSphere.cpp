#include "ColliderSphere.h"

#include "Transform.h"

ColliderSphere::ColliderSphere ( GameObject & hostObject ) :
    Collider ( hostObject, ComponentTypes::SPHERE_COLIDER ),
    m_radious ( 1.0f )
{ }

const glm::vec3 & ColliderSphere::GetCentre ( ) const
{
    return m_transform.GetPosition ( );
}

const float ColliderSphere::GetRadious ( ) const
{
    return m_radious;
}

inline void ColliderSphere::SetRadious ( const float radious )
{
    if ( radious < 0.0f )
    {
        m_radious = 0.0f;
        return;
    }

    m_radious = radious;
}

void ColliderSphere::Deserialise ( const json & data )
{
    __debugbreak ( );
}

const glm::quat & ColliderSphere::GetRotation ( ) const
{
    // Sphere's don't have a rotation. This will be ignored in Collision Manager.
    return m_transform.GetRotation( );
}

