#pragma once

#include "Component.h"

#include <glm/glm.hpp> 
#include <glm/gtx/transform.hpp> 

class Transform;

class Camera : public Component
{
public:
	enum CameraMode
	{
		PERSPECTIVE,
		ORTHOGRAPHIC
	};
	

	friend class GameObject;

private:
	Camera ( GameObject & hostObject );
	~Camera ( );

public:

#pragma region Getters and Setters


	void SetMode ( const CameraMode mode )
	{
		_mode = mode;
		_projectionMatrixIsDirty = true;
	}

	/// <summary>
	/// Gets the camera's field of view
	/// </summary>
	/// <returns>In degrees.</returns>
	const float GetFoV ( )
	{
		return glm::degrees( _fov );
	}

	/// <summary>
	/// Set the camera's field of view.
	/// </summary>
	/// <param name="fov">In degrees.</param>
	void SetFoV ( const float fov )
	{
		_fov = glm::radians( fov );
		_projectionMatrixIsDirty = true;
	}

	void SetClippingPlanes ( const glm::vec2 & planes )
	{
		_clippingPlanes = planes;
		_projectionMatrixIsDirty = true;
	}

	void SetClippingPlanes ( const float nearPlane, const float farPlane )
	{
		_clippingPlanes.x = nearPlane;
		_clippingPlanes.y = farPlane;
		_projectionMatrixIsDirty = true;
	}

	void SetOrthoRectangle ( const glm::vec4 & rect )
	{
		_orthoRectangle = rect;
		_projectionMatrixIsDirty = true;
	}
	
	void SetOrthoRectangle ( const float left, const float right, const float bottom, const float top )
	{
		_orthoRectangle.x = left;
		_orthoRectangle.y = right;
		_orthoRectangle.z = bottom;
		_orthoRectangle.w = top;
		_projectionMatrixIsDirty = true;
	}

	void SetAspectRatio ( const float aspectRatio )
	{
		_aspectRatio = aspectRatio;
		_projectionMatrixIsDirty = true;
	}

	void SetAspectRatio ( const float width, const float height )
	{
		SetAspectRatio ( width / height );
	}

#if _DEBUG

	/// <summary>
	/// Used to have an object elsewhere in the scene track the movement and rotation 
	/// of the camera to help visualise the system.
	/// </summary>
	/// <param name="t">The transform that will replicate the rotation of the camera.</param>
	/// <param name="positionOffset">How far away from the camera should the debug transform be.</param>
	void SetDebugTransform ( Transform & t, glm::vec3 & positionOffset )
	{
		m_debugTransform = &t;
		m_debugTargetPositionOffset = positionOffset;
	}

#endif

#pragma endregion

	glm::mat4 GetViewMatrix ( );
	glm::mat4 GetProjectionMatrix ( ) ;

#if _DEBUG
	void Update ( ) override;
#endif

private:
	
	CameraMode _mode;

	float _fov = glm::radians( 45.0f );
	float _aspectRatio = 16.0f / 9.0f;
	glm::vec2 _clippingPlanes = glm::vec2 ( 0.1f, 100.0f );
	glm::vec4 _orthoRectangle = glm::vec4 ( -10.0f, 10.0f, -10.0f, 10.0f );
	
	glm::mat4 _projectionMatrix;

	bool _projectionMatrixIsDirty = true;

#if _DEBUG
	Transform * m_debugTransform = nullptr;
	glm::vec3 m_debugTargetPositionOffset;
#endif

};

