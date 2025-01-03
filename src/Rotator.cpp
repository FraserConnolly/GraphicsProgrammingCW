#include "Rotator.h"
#include "Time.h"
#include "GameObject.h"

void Rotator::Update ( )
{
	if ( !glm::any ( m_axisRotation ) )
	{
		// all axis of rotations have been turned off.
		return;
	}

	if ( m_speed == 0.0f )
	{
		// nothing to do
		return;
	}

	float t = Time::GetTime ( );
	m_gameObject.GetTransform ( ).SetRotationEuler ( m_x_speed * t, m_y_speed * t, m_z_speed * t );
}

void Rotator::SetRotationAxis ( const bool x, const bool y, const bool z )
{
	m_axisRotation.x = x;
	m_axisRotation.y = y;
	m_axisRotation.z = z;

	SetRotationAxis ( m_axisRotation );
}

void Rotator::SetRotationAxis ( glm::bvec3 & axisRotation )
{
	m_axisRotation = axisRotation;

	m_x_speed = m_axisRotation.x ? m_speed : 0.0f;
	m_y_speed = m_axisRotation.y ? m_speed : 0.0f;
	m_z_speed = m_axisRotation.z ? m_speed : 0.0f;
}

void Rotator::Deserialise ( const json & data )
{
	__debugbreak ( );
}
