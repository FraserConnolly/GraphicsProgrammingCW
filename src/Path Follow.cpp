#include "Path Follow.h"

PathFollow::PathFollow ( GameObject & hostObject ) :
	Component ( hostObject, ComponentTypes::PATH_FOLLOW ),
	m_nextPointIndex ( 0 ), m_speed ( 1.0f ), m_checkDistance ( 0.1f )
{ }

void PathFollow::Update ( )
{
	if ( m_path.size ( ) == 0 && m_pathTransforms.size( ) == 0 )
	{
		return;
	}



	const glm::vec3 * nextPoint;
	size_t pointCount = 0;
	
	if ( m_path.size ( ) > 0 )
	{
		nextPoint = & ( m_path [ m_nextPointIndex ] );
		pointCount = m_path.size ( );
	}
	else
	{
		nextPoint = &( m_pathTransforms [ m_nextPointIndex ]->GetPosition( ) );
		pointCount = m_pathTransforms.size ( );
	}
	
	auto distance = glm::distance ( m_transform.GetPosition ( ), *nextPoint );


	if ( distance < m_checkDistance )
	{
		m_nextPointIndex++;

		if ( m_nextPointIndex >= pointCount )
		{
			m_nextPointIndex = 0;
		}

		return;
	}

	m_transform.LookAt ( *nextPoint );
	m_transform.Translate ( Time::GetDeltaTime ( ) * m_speed * m_transform.GetForward ( ) );
}

/// <summary>
/// Add a global coordinate that will be used as a way point.
/// If any transforms were previously added those will now be cleared.
/// </summary>
void PathFollow::AddWayPoint ( const glm::vec3 point )
{ 
	m_path.push_back ( point );
	m_pathTransforms.clear ( );
}

/// <summary>
/// Add a transform of an object that will be used as a way point.
/// If any global coordinates were previously added those will now be cleared.
/// </summary>
void PathFollow::AddWayPoint ( const Transform * transform )
{ 
	m_pathTransforms.push_back ( transform );
	m_path.clear ( );
}

void PathFollow::ClearWayPoints ( )
{ 
	m_path.clear ( );
	m_pathTransforms.clear ( );
}

void PathFollow::SetSpeed ( float speed )
{ 
	m_speed = speed;
}

const float PathFollow::GetSpeed ( )
{
	return m_speed;
}
