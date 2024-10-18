#include "GameEngine.h"

#include <iostream>
#include <string>

// Sub systems
#include "GameObjectManager.h"
#include "Time.h"
#include "Input.h"
#include "Renderer.h"
#include "CollisionManager.h"

#include "GameObject.h"
#include "CubeMap.h"

// Components
#include "Rotator.h"
#include "CameraFlyController.h"
#include "Audio Event Emitter.h"
#include "Audio Listener.h"
#include "Audio.h"
#include "Path Follow.h"

GameEngine::GameEngine ( ) { }

GameEngine::~GameEngine ( ) { }

void GameEngine::run ( )
{
	initSystems ( );
	gameLoop ( );
	shutdown ( );
}

void GameEngine::initSystems ( )
{
	_gameDisplay.initDisplay ( );

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


	m_shaderProgram = new Shader ( );
	m_shaderProgram->LoadDefaultShaders ( );
	m_shaderProgram->SetCamera ( mainCamera );

	m_shaderProgramGeo = new Shader( );
	m_shaderProgramGeo->LoadDefaultGeometoryShaders( );
	m_shaderProgramGeo->SetCamera( mainCamera );

	m_SyntyTexture = new Texture ( );
	m_SyntyTexture->LoadTexture ( "PolygonCity_Texture_01_A.png" );
	
	m_BrickTexture = new Texture ( );
	m_BrickTexture->LoadTexture ( "bricks.jpg" );

	m_SyntyMaterial = new Material ( m_shaderProgram );
	m_SyntyMaterial->SetTexture ( "diffuse", m_SyntyTexture );

	m_BrickMaterial = new Material ( m_shaderProgramGeo );
	m_BrickMaterial->SetTexture ( "diffuse", m_BrickTexture );

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
					mesh->SetMaterial ( m_BrickMaterial );
					r->SetRotationAxis ( true, !true, !true );
					emitter->LoadEvent ( "event:/Orchestra 1st Star" );
					break;
				case 1:
					mesh->loadObjModel ( "SM_Prop_Cone_02.obj" );
					mesh->SetMaterial ( m_SyntyMaterial );
					r->SetRotationAxis ( !true, true, !true );
					emitter->LoadEvent ( "event:/Orchestra 2nd Star" );
					break;
				case 2:
					mesh->loadObjModel ( "SM_Prop_Cone_01.obj" );
					mesh->SetMaterial ( m_SyntyMaterial );
					r->SetRotationAxis ( !true, !true, true );
					emitter->LoadEvent ( "event:/Orchestra 3rd Star" );
					break;
				case 3:
					mesh->loadObjModel ( "UnitCube.obj" );
					mesh->SetMaterial ( m_SyntyMaterial );
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

			mesh->SetMaterial ( m_SyntyMaterial );

			obj->GetTransform ( ).SetPosition ( ( float ) ( 0 ), 0, 0 );

			auto path = ( PathFollow * ) obj->AddComponent ( PATH_FOLLOW );

			// using world coordinates
			//for ( auto & point : points )
			//{
			//	path->AddWayPoint ( point );
			//}

			// using Transforms 
			for ( auto point : transforms )
			{
				path->AddWayPoint ( point );
			}

			path->SetSpeed ( 2.5f );

			//toFollow = &obj->GetTransform ( );
		}
	}

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
	while ( _gameState != GameState::EXIT )
	{
		Time::Service ( _gameDisplay.getTime ( ) );

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
	delete m_SyntyMaterial;
	delete m_BrickMaterial;
	delete m_shaderProgram;
	delete m_BrickTexture;
	delete m_SyntyTexture;

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
				_gameState = GameState::EXIT;
				break;
		}
	}

	if ( Input::IsKeyPressed ( SDLK_ESCAPE ) )
	{
		_gameState = GameState::EXIT;
	}
}

void GameEngine::drawGame ( )
{
	_gameDisplay.clearDisplay ( );
	Renderer::Service ( );
	if ( m_skyBox != nullptr )
	{
		m_skyBox->Draw ( );
	}
	_gameDisplay.swapBuffer ( );
}

