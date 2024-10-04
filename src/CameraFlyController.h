#pragma once
#include <GL\glew.h>

#include "GameObject.h"
#include "Component.h"
#include "Camera.h"
#include "Transform.h"
#include "Input.h"
#include "Time.h"
#include "Audio.h"
#include <SDL/SDL_keycode.h>

// Reference: https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/camera.h

// Default camera values
const float SPEED = 2.5f;
const float SENSITIVITY = 0.05f;

class CameraFlyController : public Component
{ 

    friend class GameObject;

private:
    
    // Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
    enum Camera_Movement;

	CameraFlyController ( GameObject & hostObject )
        : Component ( hostObject, ComponentTypes::CAMERA_FLY_CONTROLLER ),
        Yaw ( 0.0f ), Pitch ( 0.0f )
    {
		m_camera = nullptr;
        MovementSpeed = SPEED;
        MouseSensitivity = SENSITIVITY;
	}

public:

    void Awake ( ) override
    {
        // Technically these only need to be called on the first script.
        // But there currently isn't away of doing that.
        Input::RegisterKey ( SDLK_a ); // left
        Input::RegisterKey ( SDLK_d ); // right
        Input::RegisterKey ( SDLK_w ); // forward
        Input::RegisterKey ( SDLK_s ); // back
        Input::RegisterKey ( SDLK_q ); // down
        Input::RegisterKey ( SDLK_e ); // up
        Input::RegisterKey ( SDLK_SPACE ); // lock mouse
    }

	void SetCamera ( Camera & camera )
	{
		m_camera = &camera;
        Zoom = camera.GetFoV ( );
        Yaw = m_transform.GetRotationEuler ( ).y;
        Pitch = m_transform.GetRotationEuler ( ).x;
	}


private:

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard ( Camera_Movement direction, float deltaTime );

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement ( float xoffset, float yoffset, GLboolean constrainPitch = true );

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll ( int yoffset );

public:

    void Update ( ) override;

private:

    // Euler Angles
    float Yaw;
    float Pitch;

    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    /// <summary>
    /// Used to help debugging by preventing mouse movement from adjusting the camera.
    /// </summary>
    bool m_lockCameraRotation = true;


    Camera * m_camera;


    void UpdateCamera ( );


};

