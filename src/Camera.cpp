#include "Camera.h"

#include "GameObject.h"
#include "Transform.h"
#include "Renderer.h"

Camera::Camera ( GameObject & hostObject )
	: Component( hostObject, ComponentTypes::CAMERA ),
	_mode ( CameraMode::PERSPECTIVE )
{ }

Camera::~Camera ( )
{ }	

void Camera::Awake ( )
{
	Renderer::RegisterCamera ( this );
}

void Camera::OnDestroy ( )
{
	Renderer::DeregisterCamera ( this );
}

void Camera::Deserialise ( const json & data )
{
	// nothing to deserialise (yet)
}

glm::mat4 Camera::GetViewMatrix ( )
{
	// in lab 4 Bryan multiples the View Matrix with the Projection matrix here instead of passing the projection matrix separately as a uniform.
	// Bryan also multiplies the combined View Project matrix with the Model matrix in the application before sending the MVP (Model View Matrix) 
	// as a whole to the vertex shader.
	// I have opted to pass the three matrices to the vertex shader separately so that the GPU can do this calculation.


	// this line will result in a matrix where the camera will always look forward rather than at a defined target position in world space.
	//return glm::lookAt ( m_transform.GetPosition ( ), m_transform.GetPosition ( ) + m_transform.GetForward ( ), glm::vec3 ( 0.0f, 1.0f, 0.0f ) );
	return glm::lookAt ( m_transform.GetPosition ( ), m_transform.GetPosition ( ) + m_transform.GetForward ( ), m_transform.GetUp( ) );

}

glm::mat4 Camera::GetViewMatrixNoTranslation() {
	return glm::lookAt( glm::vec3( ), m_transform.GetForward( ), m_transform.GetUp( ) );
}

glm::mat4 Camera::GetProjectionMatrix ( )
{
	if ( _projectionMatrixIsDirty )
	{
		// recalculate projection matrix
		if ( _mode == CameraMode::PERSPECTIVE )
		{

			_projectionMatrix = glm::perspective (
				_fov, _aspectRatio,
				_clippingPlanes.x, _clippingPlanes.y );
			_projectionMatrixIsDirty = false;
		}
		else if ( _mode == CameraMode::ORTHOGRAPHIC )
		{
			_projectionMatrix = glm::ortho (
				_orthoRectangle.x,
				_orthoRectangle.y,
				_orthoRectangle.z,
				_orthoRectangle.w,
				_clippingPlanes.x, _clippingPlanes.y );
			_projectionMatrixIsDirty = false;
		}
	}

	return _projectionMatrix;
}

#if _DEBUG

	void Camera::Update ( )
	{
		if ( m_debugTransform != nullptr )
		{
			m_debugTransform->SetRotation ( m_transform.GetRotation ( ) );
			m_debugTransform->SetPosition (
				m_transform.GetPosition ( ) + m_debugTargetPositionOffset
			);
		}
	}

#endif