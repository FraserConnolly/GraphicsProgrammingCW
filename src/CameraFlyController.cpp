#include "CameraFlyController.h"

enum CameraFlyController::Camera_Movement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void CameraFlyController::ProcessKeyboard ( Camera_Movement direction, float deltaTime )
{
    auto & transform = m_gameObject.GetTransform ( );

    glm::vec3 Position = transform.GetPosition ( );

    float velocity = MovementSpeed * deltaTime;

    if ( direction == FORWARD )
        Position += transform.GetForward( ) * velocity;
    if ( direction == BACKWARD )
        Position -= transform.GetForward ( ) * velocity;
    if ( direction == LEFT )
        Position -= transform.GetRight ( ) * velocity;
    if ( direction == RIGHT )
        Position += transform.GetRight ( ) * velocity;
    if ( direction == UP )
        Position += transform.GetUp ( ) * velocity;
    if ( direction == DOWN )
        Position -= transform.GetUp ( ) * velocity;

    m_gameObject.GetTransform ( ).SetPosition ( Position );
}


// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void CameraFlyController::ProcessMouseMovement ( float xoffset, float yoffset, GLboolean constrainPitch )
{
    if ( yoffset == 0.0f && xoffset == 0.0f ) return;

    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += -xoffset;
    Pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if ( constrainPitch )
    {
        if ( Pitch > 89.0f )
            Pitch = 89.0f;
        if ( Pitch < -89.0f )
            Pitch = -89.0f;
    }
}


// processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis

void CameraFlyController::ProcessMouseScroll ( int yoffset )
{
    Zoom -= ( float ) yoffset;
    if ( Zoom < 1.0f )
        Zoom = 1.0f;
    if ( Zoom > 45.0f )
        Zoom = 45.0f;
}

void CameraFlyController::Update ( )
{
    Component::Update ( );

    // lock or unlock the camera's rotation.
    if ( Input::WasKeyPressedThisFrame ( SDLK_SPACE ) )
    {
        m_lockCameraRotation = !m_lockCameraRotation;
        Pitch = m_transform.GetRotationEuler ( ).x;
        Yaw = m_transform.GetRotationEuler ( ).y;
    }

#pragma region Camera controls

    float _deltaTime = Time::GetDeltaTime ( );

    if ( Input::IsKeyPressed ( SDLK_a ) )
    {
        ProcessKeyboard ( Camera_Movement::LEFT, _deltaTime );
    }
    if ( Input::IsKeyPressed ( SDLK_d ) )
    {
        ProcessKeyboard ( Camera_Movement::RIGHT, _deltaTime );
    }
    if ( Input::IsKeyPressed ( SDLK_w ) )
    {
        ProcessKeyboard ( Camera_Movement::FORWARD, _deltaTime );
    }
    if ( Input::IsKeyPressed ( SDLK_s ) )
    {
        ProcessKeyboard ( Camera_Movement::BACKWARD, _deltaTime );
    }
    if ( Input::IsKeyPressed ( SDLK_e ) )
    {
        ProcessKeyboard ( Camera_Movement::UP, _deltaTime );
    }
    if ( Input::IsKeyPressed ( SDLK_q ) )
    {
        ProcessKeyboard ( Camera_Movement::DOWN, _deltaTime );
    }

    if ( !m_lockCameraRotation )
    {
        auto & mouseDelta = Input::GetMouseDelta ( );
        ProcessMouseMovement ( ( float ) mouseDelta.x, ( float ) mouseDelta.y, true );

        auto & wheelDelta = Input::GetMouseWheelDelta ( );
        ProcessMouseScroll ( wheelDelta.y );

        UpdateCamera ( );
    }

#pragma endregion

}

void CameraFlyController::UpdateCamera ( )
{
    m_transform.SetRotationEulerInDegrees ( Pitch, Yaw, 0.0f );
    m_camera->SetFoV ( Zoom );
}
