#pragma once

#include <SDL\SDL.h>
#include <GL\glew.h>
#include <map>

#include "Display.h"

#if USE_DEBUG_CONSOLE
#include "ConsoleDebugScene.h"
#endif

#include "json/json.hpp"
using json = nlohmann::json;

enum class GameState
{
	PLAY, EXIT
};

class Texture;
class Shader;
class Material;
class CubeMap;
class Camera;

class GameEngine
{
public:

	GameEngine ( );
	~GameEngine ( );

	int loadGame ( const wchar_t * filename ) ;
	void run ( );

private:

	void initSystems ( );
	void processInput ( );
	void gameLoop ( );
	void shutdown ( );
	void drawGame ( );

	void LoadDirectionalLight ( json & lightData );

	GameState m_gameState = GameState::PLAY;

	json m_gameData;
	
	string m_gameName;
	int m_screenWidth = -1;
	int m_screenHeight = -1;

#if USE_DEBUG_CONSOLE
	ConsoleDebugScene _debugScene;
#endif
};

