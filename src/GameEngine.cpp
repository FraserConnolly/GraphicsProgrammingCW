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

// ComponentsMaterialSwitch
#include "Rotator.h"
#include "CameraFlyController.h"
#include "Audio Event Emitter.h"
#include "Audio Listener.h"
#include "Audio.h"
#include "Path Follow.h"
#include "ExplosionController.h"
#include "MaterialSwitch.h"
#include "NoiseController.h"

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
		m_screenWidth = m_gameData [ "Display" ][ "Width" ].get<int> ( );
		m_screenHeight = m_gameData [ "Display" ][ "Height" ].get<int> ( );
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
	m_gameDisplay.initDisplay ( m_screenWidth, m_screenHeight, m_gameName );

	SDL_SetRelativeMouseMode ( SDL_TRUE );

	Time::StartUp ( );
	Input::StartUp ( );
	Renderer::Startup ( );
	Audio::Startup ( );
	CollisionManager::Startup ( );
	GameObjectManager::Startup ( );

	Input::RegisterKey ( SDLK_ESCAPE ); // escape

#pragma region Audio

	Audio::LoadBank ( "Master.bank", FMOD_STUDIO_LOAD_BANK_NORMAL );
	Audio::LoadBank ( "Master.strings.bank", FMOD_STUDIO_LOAD_BANK_NORMAL );

	// load and play background music - this event is set to loop in FMOD.
	// note that I am note setting 3D attributes for this event as it is configured in
	// FMOD to be a 2D event.
	Audio::LoadEvent ( "event:/Lighthearted LOOP SHORT" );
	//Audio::PlayEvent ( "event:/Lighthearted LOOP SHORT" );

#pragma endregion

#pragma region Main Camera

	auto mainCameraObj = GameObjectManager::CreateObject ( );
	auto mainCamera = ( Camera * ) mainCameraObj->AddComponent ( ComponentTypes::CAMERA );

	auto flyController = ( CameraFlyController * ) mainCameraObj->AddComponent ( ComponentTypes::CAMERA_FLY_CONTROLLER );
	flyController->SetCamera ( *mainCamera );

	mainCameraObj->GetTransform ( ).SetPosition ( glm::vec3 ( 0.0f, 7.5f, 10.0f ) );
	mainCameraObj->GetTransform ( ).SetRotationEulerInDegrees ( -29, 0, 0 );
	mainCameraObj->AddComponent ( ComponentTypes::AUDIO_LISTENER );

#pragma endregion

#pragma region Skybox

	m_skyBox = new CubeMap ( );

	vector<char *> faces
	{
		"right.jpg",
		"left.jpg",
		"top.jpg",
		"bottom.jpg",
		"front.jpg",
		"back.jpg"
	};

	m_skyBox->LoadCubeMap ( faces );
	m_skyBox->SetCamera ( *mainCamera );

#pragma endregion

#pragma region Set up materials and textures

	// load shaders

	if ( m_gameData.contains ( "Shaders" ) )
	{
		try
		{
			auto shaderData = m_gameData [ "Shaders" ];
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

				m_shaders [ shaderName ] = newShader;
			}
		}
		catch ( const std::exception & )
		{
			std::cerr << "Failed to load shaders" << std::endl;
		}
	}

	// load textures

	if ( m_gameData.contains ( "ImageTextures" ) )
	{
		try
		{
			auto textureData = m_gameData [ "ImageTextures" ];
			for ( auto & texture : textureData )
			{
				auto newTexture = new Texture ( );
				auto textureName = texture [ "Name" ].get<string> ( );
				auto textureFile = texture [ "Path" ].get<string> ( );
				newTexture->LoadTexture ( textureFile.c_str ( ) );
				m_textures [ textureName ] = newTexture;
			}
		}
		catch ( const std::exception & )
		{
			std::cerr << "Failed to load textures" << std::endl;
		}
	}

	// load materials

	if ( m_gameData.contains ( "Materials" ) )
	{
		try
		{
			auto materialData = m_gameData [ "Materials" ];
			for ( auto & material : materialData )
			{
				auto materialName = material [ "Name" ].get<string> ( );

				auto shaderName = material [ "Shader" ].get<string> ( );
				auto newMaterial = new Material ( m_shaders [ shaderName ] );

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
							newMaterial->SetTexture ( name.c_str ( ) , m_textures [ textureName ] );
						}
						else
						{
							// error
							std::cerr << "Unknown uniform type: " << type << std::endl;
						}
					}
				}

				m_materials [ materialName ] = newMaterial;
			}
		}
		catch ( const std::exception & )
		{
			std::cerr << "Failed to load materials" << std::endl;
		}
	}

	// don't forget to set the camera for the shaders.
	for ( auto & shader : m_shaders )
	{
		shader.second->SetCamera ( mainCamera );
	}

	// get screen size from SDL
	//m_FBO = new FrameBuffer ( _gameDisplay.getWidth( ), _gameDisplay.getHeight( ) );


#pragma endregion

	Transform * toFollow = nullptr;

	std::vector<glm::vec3> points;
	std::vector<const Transform *> transforms;

	points.push_back ( glm::vec3 ( -2.5, 0, -2.5 ) );
	points.push_back ( glm::vec3 ( +2.5, 5, -2.5 ) );
	points.push_back ( glm::vec3 ( +2.5, 0, +2.5 ) );
	points.push_back ( glm::vec3 ( -2.5, 0, +2.5 ) );

	for ( size_t i = 0; i < points.size ( ) + 1; i++ )
	{
		auto obj = GameObjectManager::CreateObject ( );

		auto emitter = ( AudioEventEmitter * ) obj->AddComponent ( ComponentTypes::AUDIO_EVENT_EMITTER );
		emitter->SetCollisionBehaviour ( true, false );

		if ( i != points.size ( ) )
		{
			obj->GetTransform ( ).SetPosition ( points [ i ] );
			obj->GetTransform ( ).SetScale ( 1.0f );
			obj->AddComponent ( SPHERE_COLIDER );
		}
		else
		{
			// arrow
			obj->AddComponent ( SPHERE_COLIDER );
		}

		// create a mesh object
		auto mesh = ( MeshRenderer * ) obj->AddComponent ( ComponentTypes::MESH_RENDERER );

		if ( i != points.size ( ) )
		{
			transforms.push_back ( &( obj->GetTransform ( ) ) );

			auto r = ( Rotator * ) obj->AddComponent ( ComponentTypes::ROTATOR );

			switch ( i % 4 )
			{
				case 0:
					mesh->loadObjModel ( "monkey3.obj" );
					mesh->SetMaterial ( m_materials[ "Brick" ] );
					r->SetRotationAxis ( true, !true, !true );
					emitter->LoadEvent ( "event:/Orchestra 1st Star" );
					break;
				case 1:
					mesh->loadObjModel ( "SM_Prop_Cone_02.obj" );
					mesh->SetMaterial ( m_materials[ "Synty" ] );
					r->SetRotationAxis ( !true, true, !true );
					emitter->LoadEvent ( "event:/Orchestra 2nd Star" );
					break;
				case 2:
					mesh->loadObjModel ( "SM_Prop_Cone_01.obj" );
					mesh->SetMaterial ( m_materials [ "Synty" ] );
					r->SetRotationAxis ( !true, !true, true );
					emitter->LoadEvent ( "event:/Orchestra 3rd Star" );
					break;
				case 3:
					mesh->loadObjModel ( "UnitCube.obj" );
					mesh->SetMaterial ( m_materials [ "Synty" ] );
					r->SetRotationAxis ( !true, !true, !true );
					emitter->LoadEvent ( "event:/Casual Win 1" );
					break;
			}
		}

		if ( i == 1 )
		{
			// make this object player controllable.
			obj->AddComponent ( ComponentTypes::PLAYER_CONTROLLER );
		}
		
		if ( i == points.size( ) )
		{
			// This object is an arrow that will move between the four previously defined objects.
			mesh->loadObjModel ( "ArrowNegZ.obj" );

			mesh->SetMaterial ( m_materials [ "Synty" ] );

			obj->GetTransform ( ).SetPosition ( ( float ) ( 0 ), 0, 0 );

			auto path = ( PathFollow * ) obj->AddComponent ( PATH_FOLLOW );

			// using world coordinates
			for ( auto & point : points )
			{
				path->AddWayPoint ( point );
			}

			// using Transforms 
			for ( auto point : transforms )
			{
				path->AddWayPoint ( point );
			}

			path->SetSpeed ( 2.5f );

			toFollow = &obj->GetTransform ( );
		}
	}

#pragma region Suzanna

	auto obj = GameObjectManager::CreateObject ( );
	obj->GetTransform ( ).SetScale ( 2.5f );
	obj->GetTransform ( ).SetPosition ( 4.0f , 0.0f , 0.0f );

	auto mesh = ( MeshRenderer * ) obj->AddComponent ( ComponentTypes::MESH_RENDERER );
	mesh->SetMaterial ( m_materials [ "Exploding" ] );
	mesh->loadObjModel ( "monkey3.obj" );

	auto exp = ( ExplosionController * ) obj->AddComponent ( ComponentTypes::EXPLOSION_CONTROLLER );
	exp->SetMaterial ( m_materials [ "Exploding" ] );
	exp->SetSpeed ( 0.5f );

	auto switcher = ( MaterialSwitch * ) obj->AddComponent ( ComponentTypes::MATERIAL_SWITCHER );
	switcher->AddMaterial ( SDLK_1 , m_materials [ "Exploding" ] );
	switcher->AddMaterial ( SDLK_2 , m_materials [ "Brick" ] );
	switcher->AddMaterial ( SDLK_3 , m_materials [ "Synty" ] );
	switcher->AddMaterial ( SDLK_4 , m_materials [ "Fog" ] );
	switcher->AddMaterial ( SDLK_5 , m_materials [ "Rim" ] );
	switcher->AddMaterial ( SDLK_6 , m_materials [ "Toon" ] );
	switcher->AddMaterial ( SDLK_7 , m_materials [ "Planet" ] );

#pragma endregion

#pragma region Planet

	auto planetObj = GameObjectManager::CreateObject ( );
	planetObj->GetTransform ( ).SetScale ( 1.0f );
	planetObj->GetTransform ( ).SetPosition ( 0.0f , 5.0f , 0.0f );

	auto planetMesh = ( MeshRenderer * ) planetObj->AddComponent ( ComponentTypes::MESH_RENDERER );
	planetMesh->SetMaterial ( m_materials[ "Planet" ] );
	planetMesh->loadObjModel ( "ball.obj" );

	auto noise = ( NoiseController * ) planetObj->AddComponent ( ComponentTypes::NOISE_CONTROLLER );
	noise->SetMaterial ( m_materials [ "Planet" ] );
	noise->SetSpeed ( 0.5f );

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

void GameEngine::gameLoop ( )
{
	while ( m_gameState != GameState::EXIT )
	{
		Time::Service ( m_gameDisplay.getTime ( ) );

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
	for ( auto & material : m_materials )
	{
		delete material.second;
	}
	m_materials.clear ( );

	for ( auto & texture : m_textures )
	{
		delete texture.second;
	}
	m_textures.clear ( );

	for ( auto & shader : m_shaders )
	{
		delete shader.second;
	}
	m_shaders.clear ( );

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
	m_gameDisplay.clearDisplay ( );

	if ( m_FBO != nullptr )
	{
		m_FBO->Bind ( );
		Renderer::Service ( );
		if ( m_skyBox != nullptr )
		{
			m_skyBox->Draw ( );
		}
		m_FBO->Unbind ( );
	}

	if ( m_FBO != nullptr )
	{
		m_FBO->RenderQuad ( m_shaders[ "FBO" ] );
	}

	Renderer::Service ( );
	if ( m_skyBox != nullptr )
	{
		m_skyBox->Draw ( );
	}

	m_gameDisplay.swapBuffer ( );
}

