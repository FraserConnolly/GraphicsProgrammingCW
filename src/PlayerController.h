#pragma once
#include <glm/glm.hpp> 
#include "Component.h"

class PlayerController : public Component
{ 

    friend class GameObject;

private:

    PlayerController ( GameObject & hostObject );

public:

    void Awake ( ) override;

    void Start ( ) override;
    void Update ( ) override;

    void SetSpeed ( const float speed )
    {
        m_speed = speed;
    }

    const float GetSpeed ( ) const
    {
        return m_speed;
    }

    void Deserialise ( const json & data ) override;

private:

    float m_speed;
    glm::vec3 m_startingPosition;

};

