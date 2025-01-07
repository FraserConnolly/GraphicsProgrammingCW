#include <iostream>

#include "LookAt.h"
#include "GameObjectManager.h"

LookAt::LookAt ( GameObject & hostObject ) :
	Component ( hostObject, ComponentTypes::LOOK_AT )
{ }

void LookAt::Update ( )
{
	if ( m_target == nullptr )
	{
		return;
	}

	m_transform.LookAt ( m_target->GetPosition( ) );
}

void LookAt::Deserialise ( const json & data )
{
	if ( data.contains ( "Target" ) )
	{
		auto object = GameObjectManager::FindObjectByName( data[ "Target" ].get<std::string> ( ) ); 

		if ( object != nullptr )
		{
			m_target = & ( object->GetTransform( ) );
		}
		else
		{
			std::cout << "Failed to find object with name: " << data [ "Target" ].get<std::string> ( ) << std::endl;
		};
	}
}
