#include "GameObjectManager.h"

void GameObjectManager::Startup ( )
{ 
	s_instance = new GameObjectManager ( );
}

GameObjectManager::GameObjectManager ( )
{
	m_gameObjects.reserve ( GAME_OBJECT_MANAGER_CAPACITY );
	m_gameObjectsToBeDeleted.reserve ( GAME_OBJECT_MANAGER_CAPACITY );
}

void GameObjectManager::Shutdown ( )
{
	s_instance->CleanUpObjects ( );

	for ( auto & object : s_instance->m_gameObjects )
	{
		object->m_transform.SetActive ( false );
		object->RemoveAllComponentsImmediately ( );
		delete object;
	}

	s_instance->m_gameObjects.clear ( );
}

void GameObjectManager::Service ( )
{
	// call the update method on all active objects
	for ( auto & object : s_instance->m_gameObjects )
	{
		object->Update ( );

		if ( object->GetIsDestroyed ( ) )
		{
			s_instance->m_gameObjectsToBeDeleted.push_back ( object );
		}
	}

	// delete any objects that were destroyed during this frame.
	s_instance->CleanUpObjects ( );

	// clear up any components which were removed during this frame
	for ( auto & object : s_instance->m_gameObjects )
	{
		object->CleanUpComponents ( );
	}
}

GameObject * GameObjectManager::CreateObject ( )
{
	GameObject* object = new GameObject ( );
	s_instance->m_gameObjects.push_back ( object );
	return object;
}

GameObject * GameObjectManager::FindObject ( const int id )
{
	for ( auto & object : s_instance->m_gameObjects )
	{
		if ( object->GetID ( ) == id )
		{
			return object;
		}
	}

	return nullptr;
}

GameObject * GameObjectManager::FindObjectByName ( const std::string name )
{
	for ( auto & object : s_instance->m_gameObjects )
	{
		if ( object->GetName( ).compare( name ) )
		{
			return object;
		}
	}

	return nullptr;
}

void GameObjectManager::CleanUpObjects ( )
{
	for ( auto& object : m_gameObjectsToBeDeleted )
	{
		m_gameObjects.erase ( std::remove ( m_gameObjects.begin ( ), m_gameObjects.end ( ), object ), m_gameObjects.end ( ) );
		delete object;
	}
	
	m_gameObjectsToBeDeleted.clear ( );
}

GameObjectManager * GameObjectManager::s_instance = nullptr;
