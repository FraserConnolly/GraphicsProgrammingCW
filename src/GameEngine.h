#pragma once

#include <SDL\SDL.h>
#include <GL\glew.h>

#include "json/json.hpp"
using json = nlohmann::json;

#include "Display.h"

#if USE_DEBUG_CONSOLE
#include "ConsoleDebugScene.h"
#endif
#include "FrameBuffer.h"

enum class GameState
{
	PLAY, EXIT
};

class Texture;
class Shader;
class Material;
class CubeMap;

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

	GameState m_gameState = GameState::PLAY;
	Display m_gameDisplay;

	json m_gameData;
	
	string m_gameName;
	int m_screenWidth = 640;
	int m_screenHeight = 480;

	map<string, Texture *> m_textures;
	map<string, Shader *> m_shaders;
	map<string, Material *> m_materials;

	CubeMap * m_skyBox = nullptr;
	
	FrameBuffer * m_FBO = nullptr;

#if USE_DEBUG_CONSOLE
	ConsoleDebugScene _debugScene;
#endif
};

