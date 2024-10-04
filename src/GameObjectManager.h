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

private:

	GameObjectManager ( );

	void CleanUpObjects ( );

	std::vector<GameObject *> m_gameObjects;
	std::vector<GameObject *> m_gameObjectsToBeDeleted;

	static GameObjectManager * s_instance;

};

