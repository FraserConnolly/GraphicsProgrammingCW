#pragma once
#include "ColliderBase.h"

class ColliderBox :
    public Collider
{

    friend class GameObject;

    ColliderBox ( GameObject & hostObject );

public:

    const glm::vec3 & GetCentre ( ) const override;
    const glm::vec3 & GetExtents ( ) const;
    void SetExtents ( const glm::vec3 & extents );
    const glm::vec3 GetScaledHalfExtents ( ) const;
    const glm::quat & GetRotation ( ) const override;
    const Transform & GetTransform ( ) const; // just a short cut

    void Deserialise ( const json & data ) override;

private:

    glm::vec3 m_extents;
    glm::vec3 m_halfExtents;

};

