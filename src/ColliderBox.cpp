#include "ColliderBox.h"
#include "Transform.h"

ColliderBox::ColliderBox ( GameObject & hostObject ) :
    Collider ( hostObject, ComponentTypes::BOX_COLIDER ),
    m_extents ( 1.0f )
{ 
    SetExtents ( m_extents );
}

const glm::vec3 & ColliderBox::GetCentre ( ) const
{
    return m_transform.GetPosition ( );
}

const glm::vec3 & ColliderBox::GetExtents ( ) const
{
    return m_extents;
}

void ColliderBox::SetExtents ( const glm::vec3 & extents )
{
    m_extents = extents;
    m_halfExtents.x = m_extents.x / 2;
    m_halfExtents.y = m_extents.y / 2;
    m_halfExtents.z = m_extents.z / 2;
}

const glm::vec3 ColliderBox::GetScaledHalfExtents ( ) const
{
    return m_halfExtents * m_transform.GetScale ( ) ;
}

const glm::quat & ColliderBox::GetRotation ( ) const
{
    return m_transform.GetRotation ( );
}

const Transform & ColliderBox::GetTransform ( ) const
{
    return m_transform;
}

void ColliderBox::Deserialise ( const json & data )
{
    __debugbreak ( );

    if ( data.contains ( "Extents" ) )
    {
        auto extents = data [ "Extents" ];

        if ( extents.is_array ( ) && extents.size ( ) == 3 )
        {
            SetExtents ( 
                glm::vec3 ( 
                    extents [ 0 ].get<float> ( ), 
                    extents [ 1 ].get<float> ( ), 
                    extents [ 2 ].get<float> ( ) ) );
        }
	}
}
