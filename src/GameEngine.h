#pragma once

#include <SDL\SDL.h>
#include <GL\glew.h>

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

	void run ( );

private:

	void initSystems ( );
	void processInput ( );
	void gameLoop ( );
	void shutdown ( );
	void drawGame ( );

	GameState _gameState = GameState::PLAY;
	Display _gameDisplay;

	Material * m_FogMaterial = nullptr;
	Material * m_RimMaterial = nullptr;
	Material * m_ToonMaterial = nullptr;
	Material * m_BrickMaterial = nullptr;
	Material * m_SyntyMaterial = nullptr;
	Material * m_PlanetMaterial = nullptr;

	Texture * m_SyntyTexture = nullptr;
	Texture * m_BrickTexture = nullptr;
	Texture * m_Lava = nullptr;
	Texture * m_Noise = nullptr;
	
	Shader * m_shaderProgram = nullptr;
	Shader * m_shaderProgramGeo = nullptr;
	Shader * m_shaderProgramRim = nullptr;
	Shader * m_shaderProgramFog = nullptr;
	Shader * m_shaderProgramToon = nullptr;
	Shader * m_shaderProgramFBO = nullptr;
	Shader * m_shaderProgramNoise = nullptr;
	
	CubeMap * m_skyBox = nullptr;
	
	FrameBuffer * m_FBO = nullptr;

#if USE_DEBUG_CONSOLE
	ConsoleDebugScene _debugScene;
#endif
};

