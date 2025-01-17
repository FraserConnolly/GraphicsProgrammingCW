#include "GameEngine.h"

#include <iostream>
#include <string>
#include <fstream>


// Sub systems
#include "GameObjectManager.h"
#include "Time.h"
#include "Input.h"
#include "Renderer.h"
#include "CollisionManager.h"

#include "GameObject.h"
#include "CubeMap.h"
#include "FrameBuffer.h"
#include "Material.h"
#include "Shader.h"
#include "Texture.h"
#include "DirectionalLight.h"

// ComponentsMaterialSwitch
#include "Audio Listener.h"
#include "Audio.h"

using json = nlohmann::json;

GameEngine::GameEngine ( ) { }

GameEngine::~GameEngine ( ) { }

int GameEngine::loadGame ( const wchar_t * filename )
{
	// load game data from file

	std::ifstream gameFile ( filename );
	
	if ( !gameFile.is_open ( ) ) {
		std::cerr << "Failed to open game file: " << filename << std::endl;
		return 1;
	}

	try
	{
		m_gameData = json::parse ( gameFile );

		gameFile.close ( );

		// load game data into game engine
		m_gameName = m_gameData [ "GameName" ].get<string> ( );
		if ( m_gameData.contains ( "Display" ) )
		{
			if ( m_gameData [ "Display" ].contains ( "Width" ) && m_gameData [ "Display" ].contains ( "Height" ) )
			{
				m_screenWidth = m_gameData [ "Display" ][ "Width" ].get<int> ( );
				m_screenHeight = m_gameData [ "Display" ][ "Height" ].get<int> ( );
			}
		}
	}
	catch ( const std::exception & )
	{
		gameFile.close ( );
		return 2;
	}

	return 0;
}

void GameEngine::run ( )
{
	initSystems ( );
	gameLoop ( );
	shutdown ( );
}

void GameEngine::initSystems ( )
{

	Time::StartUp ( );
	Input::StartUp ( );
	Renderer::Startup ( m_screenWidth , m_screenHeight , m_gameName );
	Audio::Startup ( );
	CollisionManager::Startup ( );
	GameObjectManager::Startup ( );

	SDL_SetRelativeMouseMode ( SDL_TRUE );

	Input::RegisterKey ( SDLK_ESCAPE ); // escape

#pragma region Audio

	Audio::LoadBank ( "Master.bank", FMOD_STUDIO_LOAD_BANK_NORMAL );
	Audio::LoadBank ( "Master.strings.bank", FMOD_STUDIO_LOAD_BANK_NORMAL );

#pragma endregion

#pragma region Load shaders, materials, and textures

	// load shaders

	if ( m_gameData.contains ( "Shaders" ) )
	{
		try
		{
			auto & shaderData = m_gameData [ "Shaders" ];
			for ( auto & shader : shaderData )
			{
				auto newShader = new Shader ( );

				auto shaderName = shader [ "Name" ].get<string> ( );
				auto vertexShader = shader [ "Vertex" ].get<string> ( );
				auto fragmentShader = shader [ "Fragment" ].get<string> ( );

				if ( shader.contains ( "Geometry" ) )
				{
					auto geometryShader = shader [ "Geometry" ].get<string> ( );
					newShader->LoadShaders ( vertexShader.c_str ( ) , geometryShader.c_str ( ) , fragmentShader.c_str ( ) );
				}
				else
				{
					newShader->LoadShaders ( vertexShader.c_str ( ) , fragmentShader.c_str ( ) );
				}

				Renderer::RegisterShader ( shaderName , newShader );
			}
		}
		catch ( const std::exception & )
		{
			std::cerr << "Failed to load shaders" << std::endl;
		}
	}

	// load image textures

	if ( m_gameData.contains ( "ImageTextures" ) )
	{
		try
		{
			auto & textureData = m_gameData [ "ImageTextures" ];
			for ( auto & texture : textureData )
			{
				auto newTexture = new Texture ( );
				auto textureName = texture [ "Name" ].get<string> ( );
				auto textureFile = texture [ "Path" ].get<string> ( );
				newTexture->LoadTexture ( textureFile.c_str ( ) );
				Renderer::RegisterTexture ( textureName , newTexture );
			}
		}
		catch ( const std::exception & )
		{
			std::cerr << "Failed to load textures" << std::endl;
		}
	}

	// load cube map textures

	if ( m_gameData.contains ( "CubeMaps" ) )
	{
		try
		{
			auto & cubeMapData = m_gameData [ "CubeMaps" ];
			for ( auto & texture : cubeMapData )
			{
				auto newTexture = new Texture ( );
				auto textureName = texture [ "Name" ].get<string> ( );
				auto & textureFiles = texture [ "Paths" ];
				vector<char *> faces;

				// Loop through the JSON array and convert each string to char*
				for ( const auto & path : textureFiles ) {
					// Convert each string to char* and push to the vector
					// We use strdup here to create a copy of the string
					faces.push_back ( _strdup ( path.get<std::string> ( ).c_str ( ) ) );
				}

				newTexture->LoadCubeMap ( faces );
				Renderer::RegisterTexture ( textureName, newTexture );
			}
		}
		catch ( const std::exception & )
		{
			std::cerr << "Failed to load textures" << std::endl;
		}
	}

	// create frame buffers

	if ( m_gameData.contains ( "FrameBuffers" ) )
	{
		try
		{
			auto & fbData = m_gameData [ "FrameBuffers" ];
			for ( auto & frameBuffer : fbData )
			{
				if ( !frameBuffer.contains ( "Name" ) || !frameBuffer.contains ( "Texture" ) )
				{
					std::cerr << "Frame buffer missing name or texture." << std::endl;
					continue;
				}

				auto frameBufferName = frameBuffer [ "Name" ].get<string> ( );
				auto textureName = frameBuffer [ "Texture" ].get<string> ( );

				// default screen size comes from Display
				int frameBufferWidth = Renderer::GetScreenWidth ( );
				int frameBufferHeight = Renderer::GetScreenHeight ( );

				if ( frameBuffer.contains ( "Width" ) )
				{
					frameBufferWidth = frameBuffer [ "Width" ].get<int> ( );
				}

				if ( frameBuffer.contains ( "Height" ) )
				{
					frameBufferHeight = frameBuffer [ "Height" ].get<int> ( );
				}

				
				bool depthMapOnly = false;
				if ( frameBuffer.contains ( "DepthMapOnly" ) )
				{
					depthMapOnly = frameBuffer [ "DepthMapOnly" ].get<bool> ( );
				}

				auto newFrameBuffer = new FrameBuffer ( frameBufferWidth , frameBufferHeight , textureName, depthMapOnly );
				Renderer::RegisterFrameBuffer ( frameBufferName , newFrameBuffer );
			}
		}
		catch ( const std::exception & )
		{
			std::cerr << "Failed to load frame buffers." << std::endl;
		}
	}

	// load materials
	
	if ( m_gameData.contains ( "Materials" ) )
	{
		try
		{
			auto & materialData = m_gameData [ "Materials" ];
			for ( auto & material : materialData )
			{
				auto materialName = material [ "Name" ].get<string> ( );

				auto shaderName = material [ "Shader" ].get<string> ( );
				auto newMaterial = new Material ( Renderer::GetShader ( shaderName ) );

				if ( material.contains ( "Uniforms" ) )
				{
					for ( auto & uniform : material [ "Uniforms" ] )
					{
						auto type = uniform [ "Type" ].get<string> ( );
						auto name = uniform [ "Name" ].get<string> ( );

						if ( type == "float" )
						{
							newMaterial->SetFloatByName ( name.c_str ( ) , uniform [ "Value" ].get<float> ( ) );
						}
						else if ( type == "vec3" )
						{
							newMaterial->SetFloat3ByName ( name.c_str ( ) ,
								uniform [ "Value" ][ 0 ].get<float> ( ) ,
								uniform [ "Value" ][ 1 ].get<float> ( ) ,
								uniform [ "Value" ][ 2 ].get<float> ( ) );
						}
						else if ( type == "Texture" )
						{
							auto textureName = uniform [ "Value" ].get<string> ( );
							newMaterial->SetTexture ( name.c_str ( ) , Renderer::GetTexture ( textureName ) );
						}
						else
						{
							// error
							std::cerr << "Unknown uniform type: " << type << std::endl;
						}
					}
				}

				Renderer::RegisterMaterial ( materialName , newMaterial );
			}
		}
		catch ( const std::exception & )
		{
			std::cerr << "Failed to load materials" << std::endl;
		}
	}

#pragma endregion

#pragma region Skybox

	if ( m_gameData.contains ( "Skybox" ) )
	{
		try
		{
			auto & skyboxData = m_gameData [ "Skybox" ];
			CubeMap * skyBox = new CubeMap (
				Renderer::GetShader ( skyboxData [ "Shader" ] ) ,
				Renderer::GetTexture ( skyboxData [ "Texture" ] ) );

			Renderer::SetSkybox ( skyBox );
		}
		catch ( const std::exception & )
		{
			std::cerr << "Failed to load skybox" << std::endl;
		}
	}

#pragma endregion

#pragma region Lighting

	// load lighting

	if ( m_gameData.contains ( "DirectionalLight" ) )
	{
		LoadDirectionalLight ( m_gameData [ "DirectionalLight" ] );
	}

#pragma endregion

#pragma region Create game objects

	if ( m_gameData.contains ( "Objects" ) )
	{
		// this allows all new objects to be created and named before components are added.
		map<GameObject *, json> objectComponents;

		try
		{
			auto & objects = m_gameData [ "Objects" ];
			for ( auto & object : objects )
			{
				// Note, object name is currently not implemented.

				auto obj = GameObjectManager::CreateObject ( );
				
				if ( object.contains ( "Name" ) )
				{
					obj->SetName ( object [ "Name" ].get<string> ( ) );
				}

				if ( object.contains ( "Position" ) )
				{
					auto & position = object [ "Position" ];
					obj->GetTransform ( ).SetPosition ( position [ 0 ].get<float> ( ) ,
						position [ 1 ].get<float> ( ) ,
						position [ 2 ].get<float> ( ) );
				}

				if ( object.contains ( "Rotation" ) )
				{
					auto & rotation = object [ "Rotation" ];
					obj->GetTransform ( ).SetRotationEulerInDegrees ( rotation [ 0 ].get<float> ( ) ,
												rotation [ 1 ].get<float> ( ) ,
												rotation [ 2 ].get<float> ( ) );
				}

				if ( object.contains ( "Scale" ) )
				{
					auto & scale = object [ "Scale" ];
					obj->GetTransform ( ).SetScale ( scale [ 0 ].get<float> ( ) ,
						scale [ 1 ].get<float> ( ) ,
						scale [ 2 ].get<float> ( ) );
				}

				if ( object.contains ( "Active" ) )
				{
					obj->SetActive ( object [ "Active" ].get<bool> ( ) );
				}

				if ( object.contains ( "Components" ) )
				{
					objectComponents[ obj ] = object [ "Components" ];
				}
			}

			// add components to objects
			for ( auto & object : objectComponents )
			{
				auto & obj = object.first;			// the game object	
				auto & components = object.second;	// the json array of components

				for ( auto & component : components )
				{
					auto type = component [ "Type" ].get<string> ( );
					auto newComponent = obj->AddComponent ( type );
					newComponent->Deserialise ( component );
				}
			}
		}
		catch ( const std::exception & )
		{
			std::cerr << "Failed to load objects" << std::endl;
		}

		objectComponents.clear ( );
	}

#pragma endregion

#if USE_DEBUG_CONSOLE
	_debugScene.initaliseScene ( 0 );
	_debugScene.SetTransformToMonitor ( mainCameraObj->GetTransform ( ) );
	if ( toFollow != nullptr )
	{
		_debugScene.SetTransformToMonitor ( *toFollow );
	}
#endif
}

void GameEngine::LoadDirectionalLight ( json & lightData )
{
	try
	{
		auto & shaderName		= lightData [ "Shader"		].get<std::string> ( );
		auto & frameBufferName	= lightData [ "FrameBuffer" ].get<std::string> ( );

		auto shader = Renderer::GetShader ( shaderName );
		auto depthBuffer = Renderer::GetFrameBuffer ( frameBufferName );

		if ( shader == nullptr || depthBuffer == nullptr )
		{
			std::cerr << "Failed to load directional light" << std::endl;
			return;
		}

		auto light = new DirectionalLight ( depthBuffer , shader );

		if ( lightData.contains ( "Direction" ) )
		{
			light->SetDirection ( glm::vec3 (
				lightData [ "Direction" ][ 0 ].get<float> ( ) ,
				lightData [ "Direction" ][ 1 ].get<float> ( ) ,
				lightData [ "Direction" ][ 2 ].get<float> ( ) ) );
		}

		if ( lightData.contains ( "Colour" ) )
		{
			light->SetColor ( glm::vec3 (
				lightData [ "Colour" ][ 0 ].get<float> ( ) ,
				lightData [ "Colour" ][ 1 ].get<float> ( ) ,
				lightData [ "Colour" ][ 2 ].get<float> ( ) ) );
		}

		if ( lightData.contains ( "Intensity" ) )
		{
			light->SetIntensity ( lightData [ "Intensity" ].get<float> ( ) );
		}

		Renderer::SetDirectionalLight ( light );
	}
	catch ( const std::exception & )
	{
		std::cerr << "Failed to load directional light" << std::endl;
	}
}

void GameEngine::gameLoop ( )
{
	while ( m_gameState != GameState::EXIT )
	{
		Time::Service ( Renderer::GetTime ( ) );

		processInput ( );
		CollisionManager::Service ( );
		GameObjectManager::Service ( );
		Audio::Service ( );

		drawGame ( );

#if USE_DEBUG_CONSOLE
		_debugScene.processFrame ( );
#endif
	}
}

void GameEngine::shutdown ( )
{
	GameObjectManager::Shutdown ( );
	CollisionManager::Shutdown ( );
	Audio::Shutdown ( );
	Renderer::Shutdown ( );
	Input::Shutdown ( );
	Time::Shutdown ( );
	SDL_SetRelativeMouseMode ( SDL_FALSE );
}

void GameEngine::processInput ( )
{
	Input::Service ( );

	SDL_Event eventData;

	while ( SDL_PollEvent ( &eventData ) )
	{
		switch ( eventData.type )
		{
			case SDL_KEYDOWN:
			case SDL_KEYUP:
				Input::ProcessKeyEvent ( eventData.key.keysym.sym, ( eventData.key.state == SDL_PRESSED ) );
				break;
			case SDL_MOUSEWHEEL:
				// note that in later version of SDL a float precision version of this data is available.
				Input::ProcessWheel ( eventData.wheel.x, eventData.wheel.y );
				break;
			case SDL_QUIT:
				m_gameState = GameState::EXIT;
				break;
		}
	}

	if ( Input::IsKeyPressed ( SDLK_ESCAPE ) )
	{
		m_gameState = GameState::EXIT;
	}
}

void GameEngine::drawGame ( )
{
	Renderer::Service ( );
}

