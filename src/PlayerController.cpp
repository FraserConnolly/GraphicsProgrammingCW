#include "PlayerController.h"
#include <SDL/SDL_keycode.h>
#include "Transform.h"
#include "Input.h"
#include "Time.h"

PlayerController::PlayerController ( GameObject & hostObject )
    : Component ( hostObject, ComponentTypes::PLAYER_CONTROLLER ),
    m_speed ( 1.0f )
{
}


inline void PlayerController::Awake ( )
{
    // Technically these only need to be called on the first script.
    // But there currently isn't away of doing that.
    Input::RegisterKey ( SDLK_LEFT ); // left
    Input::RegisterKey ( SDLK_RIGHT ); // right
    Input::RegisterKey ( SDLK_UP ); // forward
    Input::RegisterKey ( SDLK_DOWN ); // back
    Input::RegisterKey ( SDLK_END ); // down
    Input::RegisterKey ( SDLK_RSHIFT ); // away
    Input::RegisterKey ( SDLK_RCTRL ); // near
    Input::RegisterKey ( SDLK_END ); // near
}

void PlayerController::Start ( )
{ 
    m_startingPosition = m_transform.GetPosition ( );
}

void PlayerController::Update ( )
{ 
    float deltaTime = Time::GetDeltaTime ( );
    float velocity = m_speed * deltaTime;
    glm::vec3 Position = m_transform.GetPosition ( );

    if ( Input::IsKeyPressed ( SDLK_LEFT ) )
    {
        Position -= glm::vec3 ( 1, 0, 0 ) * velocity;
    }
    if ( Input::IsKeyPressed ( SDLK_RIGHT ) )
    {
        Position += glm::vec3 ( 1, 0, 0 ) * velocity;
    }
    if ( Input::IsKeyPressed ( SDLK_UP ) )
    {
        Position += glm::vec3 ( 0, 1, 0 ) * velocity;
    }
    if ( Input::IsKeyPressed ( SDLK_DOWN ) )
    {
        Position -= glm::vec3 ( 0, 1, 0 ) * velocity;
    }
    if ( Input::IsKeyPressed ( SDLK_RCTRL ) )
    {
        Position += glm::vec3 ( 0, 0, -1 ) * velocity;
    }
    if ( Input::IsKeyPressed ( SDLK_RSHIFT ) )
    {
        Position -= glm::vec3( 0,0,-1) * velocity;
    }

    if ( Input::WasKeyPressedThisFrame ( SDLK_END ) )
    {
        m_transform.SetPosition ( m_startingPosition );
        return;
    }

    m_transform.SetPosition ( Position );
}

void PlayerController::Deserialise ( const json & data )
{
    __debugbreak ( );
}

