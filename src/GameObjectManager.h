#pragma once
#include <vector>
#include "GameObject.h"

#define GAME_OBJECT_MANAGER_CAPACITY 100

class GameObjectManager
{ 
public:

	static void Startup ( );
	static void Service ( );
	static void Shutdown ( );

	static GameObject * CreateObject( );

	static GameObject * FindObject ( const int id );
	static GameObject * FindObjectByName ( const std::string name );

private:

	GameObjectManager ( );

	void CleanUpObjects ( );

	std::vector<GameObject *> m_gameObjects;
	std::vector<GameObject *> m_gameObjectsToBeDeleted;

	static GameObjectManager * s_instance;

};

