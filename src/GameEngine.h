#pragma once

#include <SDL\SDL.h>
#include <GL\glew.h>

#include "Display.h"

#if USE_DEBUG_CONSOLE
#include "ConsoleDebugScene.h"
#endif

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

	void run ( );

private:

	void initSystems ( );
	void processInput ( );
	void gameLoop ( );
	void shutdown ( );
	void drawGame ( );

	GameState _gameState = GameState::PLAY;
	Display _gameDisplay;

	Material * m_BrickMaterial = nullptr;
	Material * m_SyntyMaterial = nullptr;
	Texture * m_SyntyTexture = nullptr;
	Texture * m_BrickTexture = nullptr;
	Shader * m_shaderProgram = nullptr;
	Shader * m_shaderProgramGeo = nullptr;
	CubeMap * m_skyBox = nullptr;

#if USE_DEBUG_CONSOLE
	ConsoleDebugScene _debugScene;
#endif
};

