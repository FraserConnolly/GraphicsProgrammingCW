#include <iostream>

#include "TransformFollow.h"
#include "GameObjectManager.h"

TransformFollow::TransformFollow ( GameObject & hostObject ) :
	Component ( hostObject , ComponentTypes::TRANSFORM_FOLLOW )
{ }

void TransformFollow::Update ( )
{
	if ( m_target == nullptr )
	{
		return;
	}

	auto rot = m_target->GetRotation ( );

	auto rotatedOffset = rot * m_offset;

	m_transform.SetPosition ( m_target->GetPosition ( ) + rotatedOffset );
	
	if ( m_rotationOffset != glm::vec3 ( 0.0f ) )
	{
		auto offset = glm::quat ( m_rotationOffset );
		m_transform.SetRotation ( glm::normalize ( rot * offset ) );
	}
	else 
	{
		m_transform.SetRotation ( m_target->GetRotation ( ) );
	}
}

void TransformFollow::Deserialise ( const json & data )
{
	if ( data.contains ( "Target" ) )
	{
		auto object = GameObjectManager::FindObjectByName ( data [ "Target" ].get<std::string> ( ) );

		if ( object != nullptr )
		{
			m_target = &( object->GetTransform ( ) );
		}
		else
		{
			std::cout << "Failed to find object with name: " << data [ "Target" ].get<std::string> ( ) << std::endl;
		};
	}

	if ( data.contains ( "Offset" ) && data["Offset" ].is_array() && data["Offset" ].size() == 3 )
	{
		auto offset = glm::vec3 ( 
			data [ "Offset" ][ 0 ].get<float> ( ) , 
			data [ "Offset" ][ 1 ].get<float> ( ) , 
			data [ "Offset" ][ 2 ].get<float> ( ) );
		SetOffset ( offset );
	}

	if ( data.contains ( "Rotation" ) && data [ "Rotation" ].is_array( ) && data [ "Rotation" ].size ( ) == 3 )
	{
		auto rotation = glm::vec3 ( 
			data [ "Rotation" ][ 0 ].get<float> ( ) , 
			data [ "Rotation" ][ 1 ].get<float> ( ) , 
			data [ "Rotation" ][ 2 ].get<float> ( ) );
		SetRotationOffset ( rotation );
	}
}
