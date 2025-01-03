#pragma once
#include <glm/glm.hpp> 
#include <glm/gtx/transform.hpp> 
#include <glm/gtx/euler_angles.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Component.h"
#include <vector>

class Transform : public Component
{
public: 

#pragma region Getters

	inline glm::mat4 & GetModel ( ) const
	{
		ApplyNewData ( );
		return m_modelMatrix;
	}

	inline const glm::vec3 & GetPosition ( ) const
	{
		return m_position;
	} 

	inline const glm::quat & GetRotation ( ) const
	{ 
		return m_rotation; 
	} 

	/// <returns>Euler angles in degrees.</returns>
	inline const glm::vec3 & GetRotationEuler ( ) const
	{
		ApplyNewData ( );
		return m_rotationEuler;
	}

	inline const glm::vec3 & GetScale ( ) const
	{ 
		return m_scale; 
	}

	inline const glm::vec3 & GetForward ( ) const
	{
		ApplyNewData ( );
		return m_forward;
	}

	inline const glm::vec3 & GetUp ( ) const
	{
		ApplyNewData ( );
		return m_up;
	}

	inline const glm::vec3 & GetRight ( ) const
	{
		ApplyNewData ( );
		return m_right;
	}

#pragma endregion

#pragma region Setters

	inline void SetPosition ( const glm::vec3 & pos )
	{
		m_position = pos;
		SetDirty ( );
	}

	inline void SetPosition ( const float x, const float y, const float z )
	{
		m_position.x = x;
		m_position.y = y;
		m_position.z = z;
		SetDirty ( );
	}

	inline void Translate ( float x, float y, float z )
	{
		Translate ( glm::vec3 ( x, y, z ) );
	}

	inline void Translate ( glm::vec3 & offset )
	{
		m_position += offset;
		SetDirty ( );
	}

	inline void SetRotation( const glm::quat & rot ) 
	{
		m_rotation = rot;
		SetDirty ( );
	}


	inline void SetRotationEuler ( const glm::vec3 & eulerAngles )
	{
		m_rotation = glm::quat ( eulerAngles );
		SetDirty ( );
	}

	inline void SetRotationEuler ( const float x, const float y, const float z )
	{
		SetRotationEuler ( glm::vec3 ( x, y, z ) );
	}
	
	inline void SetRotationEulerInDegrees ( const float x, const float y, const float z )
	{
		SetRotationEuler ( glm::vec3 (
			glm::radians ( x ),
			glm::radians ( y ),
			glm::radians ( z ) ) );
	}

	inline void SetRotationNormalisedAxis ( glm::vec3 & forward, glm::vec3 & up )
	{
		m_rotation = glm::quat ( forward, up );
		SetDirty ( );
	}

	// This function was taken significantly written by ChatGPT. 
	// Please see Chat GPT Capture 1.png and Chat GPT Capture 2.png for query and response.
	// I don't really understand why this works but I lost a day trying to make it work
	// directly with the quaternion and failed miserably so going via Euler was the compromise.
	inline void LookAt ( const glm::vec3 & target )
	{
		glm::vec3 direction = glm::normalize ( target - m_position );

		// Calculate yaw angle (horizontal rotation around the y-axis)
		float yaw = atan2 ( -direction.z, direction.x ) - glm::pi<float> ( ) / 2.0f; // Adding 90 degrees to adjust for convention

		// Calculate pitch angle (vertical rotation around the x-axis)
		float pitch = asin ( direction.y );

		SetRotationEuler ( pitch, yaw, 0.0f );
	}

	inline void SetScale ( const glm::vec3 & scale )
	{
		m_scale = scale;
		SetDirty ( );
	}

	inline void SetScale ( const float x, const float y, const float z )
	{
		m_scale.x = x;
		m_scale.y = y;
		m_scale.z = z;
		SetDirty ( );
	}

	inline void SetScale ( const float s )
	{
		m_scale.x = s;
		m_scale.y = s;
		m_scale.z = s;
		SetDirty ( );
	}

#pragma endregion

#pragma region Hierarchy

	void SetParent ( Transform * const parent )
	{
		if ( m_parent != nullptr )
		{
			m_parent->RemoveChild ( this );
			// remove use from the existing parent's list of children
		}

		m_parent = parent;
		m_parent->AddChild ( this );
		SetDirty ( );
	}

#pragma endregion

	// Inherited via Component
	void Reset ( ) override
	{
		SetPosition ( 0.0f, 0.0f, 0.0f );
		SetRotationEuler ( 0.0f, 0.0f, 0.0f );
		SetScale ( 1.0f, 1.0f, 1.0f );
	}

	void OnDestroy ( ) override;

	void OnEnable ( ) override;

	void OnDisable ( ) override;

	void Deserialise ( const json & data ) override;

protected: 

	void SetDirty ( )
	{
		if ( m_isDirty )
		{
			// already flagged as dirty
			return;
		}

		m_isDirty = true;

		// set the dirty flag for all children too
		for ( auto child : m_children )
		{
			child->SetDirty ( );
		}
	}

#pragma region Hierarchy

	void AddChild ( Transform * const pChild );

	void RemoveChild ( Transform * pChild );

	void Destroy ( );

#pragma endregion

private: 

	friend class GameObject;

	Transform ( GameObject & hostObject, const glm::vec3 & pos = glm::vec3 ( ), const glm::vec3 & rot = glm::vec3 ( ), const glm::vec3 & scale = glm::vec3 ( 1.0f, 1.0f, 1.0f ) ) 
		: Component ( hostObject, ComponentTypes::TRANSFORM )
	{
		SetPosition ( pos );
		SetRotation ( rot );
		SetScale ( scale );
	}

	mutable bool m_isDirty;

	glm::vec3 m_position; 
	glm::quat m_rotation;
	glm::vec3 m_scale; 
	
	// These member variables are holding cached values 
	// that may be updated when they are retrieved which is why
	// they have the mutable keyword.
	mutable glm::vec3 m_forward = WorldForward; 
	mutable glm::vec3 m_up = WorldUp;
	mutable glm::vec3 m_right = WorldRight;
	
	mutable glm::vec3 m_rotationEuler; // for reading only - set by apply data.
	mutable glm::mat4 m_modelMatrix;

	Transform * m_parent = nullptr;
	std::vector<Transform *> m_children;

	/// <summary>
	/// Updates the cached values.
	/// </summary>
	void ApplyNewData ( ) const;

	// Right handed world unit vectors.
	const glm::vec4 WorldUp = glm::vec4 ( 0.0f, 1.0f, 0.0f, 0.0f );
	const glm::vec4 WorldForward = glm::vec4 ( 0.0f, 0.0f, -1.0f, 0.0f );
	const glm::vec4 WorldRight = glm::vec4 ( 1.0f, 0.0f, 0.0f, 0.0f );

};