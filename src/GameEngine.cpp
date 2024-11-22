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

	m_shaderProgramGeo = new Shader ( );
	m_shaderProgramGeo->LoadDefaultGeometoryShaders ( );
	m_shaderProgramGeo->SetCamera ( mainCamera );

	m_shaderProgramFog = new Shader ( );
	m_shaderProgramFog->LoadShaders ( "fogShader.vert", "fogShader.frag" );
	m_shaderProgramFog->SetCamera ( mainCamera );

	m_shaderProgramRim = new Shader ( );
	m_shaderProgramRim->LoadShaders ( "shaderRim.vert", "shaderRim.frag" );
	m_shaderProgramRim->SetCamera ( mainCamera );

	m_shaderProgramToon = new Shader ( );
	m_shaderProgramToon->LoadShaders ( "shaderToon.vert", "shaderToon.frag" );
	m_shaderProgramToon->SetCamera ( mainCamera );

	m_shaderProgramFBO = new Shader ( );
	m_shaderProgramFBO->LoadShaders ( "shaderFBO.vert" , "edgeDetectionShader.frag" );
	m_shaderProgramFBO->SetCamera ( mainCamera );

	m_shaderProgramNoise = new Shader ( );
	m_shaderProgramNoise->LoadShaders ( "shaderNoise.vert" , "shaderNoise.frag" );
	m_shaderProgramNoise->SetCamera ( mainCamera );

	m_SyntyTexture = new Texture ( );
	m_SyntyTexture->LoadTexture ( "PolygonCity_Texture_01_A.png" );
	
	m_BrickTexture = new Texture ( );
	m_BrickTexture->LoadTexture ( "bricks.jpg" );

	m_Lava = new Texture ( );
	m_Lava->LoadTexture ( "lava.jpg" );

	m_Noise = new Texture ( );
	m_Noise->LoadTexture ( "noise.png" );

	m_BrickTexture = new Texture ( );
	m_BrickTexture->LoadTexture ( "bricks.jpg" );

	m_SyntyMaterial = new Material ( m_shaderProgram );
	m_SyntyMaterial->SetTexture ( "diffuse", m_SyntyTexture );

	m_BrickMaterial = new Material ( m_shaderProgram );
	m_BrickMaterial->SetTexture ( "diffuse", m_BrickTexture );

	m_FogMaterial	= new Material ( m_shaderProgramFog );
	m_FogMaterial->SetFloat3ByName	( "fogColor", 0.5f, 0.1f, 0.1f );
	m_FogMaterial->SetFloatByName	( "maxDist", 50.0f);
	m_FogMaterial->SetFloatByName	( "minDist",  5.0f);
	m_FogMaterial->SetTexture		( "diffuse", m_BrickTexture );

	m_ToonMaterial = new Material	( m_shaderProgramToon );
	m_ToonMaterial->SetTexture		( "diffuse", m_BrickTexture );
	m_ToonMaterial->SetFloat3ByName	( "lightDir", 0.5f, 0.1f, 0.1f );

	m_RimMaterial = new Material	( m_shaderProgramRim );
	m_RimMaterial->SetTexture		( "diffuse" , m_BrickTexture );
	m_RimMaterial->SetFloat3ByName	( "lightDir" , 0.5f , 0.1f , 0.1f );

	m_PlanetMaterial = new Material ( m_shaderProgramNoise );
	m_PlanetMaterial->SetTexture ( "texture1" , m_Noise );
	m_PlanetMaterial->SetTexture ( "texture2" , m_Lava );

	// get screen size from SDL
	//m_FBO = new FrameBuffer ( _gameDisplay.getWidth( ), _gameDisplay.getHeight( ) );


#pragma endregion

	Transform * toFollow = nullptr;

	std::vector<glm::vec3> points;
	std::vector<const Transform *> transforms;

	//points.push_back ( glm::vec3 ( -2.5, 0, -2.5 ) );
	//points.push_back ( glm::vec3 ( +2.5, 5, -2.5 ) );
	//points.push_back ( glm::vec3 ( +2.5, 0, +2.5 ) );
	//points.push_back ( glm::vec3 ( -2.5, 0, +2.5 ) );

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

#pragma region Suzanna

	//auto obj = GameObjectManager::CreateObject ( );
	//obj->GetTransform ( ).SetScale ( 2.5f );
	//obj->GetTransform ( ).SetPosition ( 4.0f , 0.0f , 0.0f );

	//auto explodingMaterial = new Material ( m_shaderProgramGeo );
	//explodingMaterial->SetTexture ( "diffuse" , m_BrickTexture );

	//auto mesh = ( MeshRenderer * ) obj->AddComponent ( ComponentTypes::MESH_RENDERER );
	//mesh->SetMaterial ( explodingMaterial );
	//mesh->loadObjModel ( "monkey3.obj" );

	//auto exp = ( ExplosionController * ) obj->AddComponent ( ComponentTypes::EXPLOSION_CONTROLLER );
	//exp->SetMaterial ( explodingMaterial );
	//exp->SetSpeed ( 0.5f );

	//auto switcher = ( MaterialSwitch * ) obj->AddComponent ( ComponentTypes::MATERIAL_SWITCHER );
	//switcher->AddMaterial ( SDLK_1 , explodingMaterial );
	//switcher->AddMaterial ( SDLK_2 , m_BrickMaterial );
	//switcher->AddMaterial ( SDLK_3 , m_SyntyMaterial );
	//switcher->AddMaterial ( SDLK_4 , m_FogMaterial );
	//switcher->AddMaterial ( SDLK_5 , m_RimMaterial );
	//switcher->AddMaterial ( SDLK_6 , m_ToonMaterial );

#pragma endregion

#pragma region Planet

	auto planetObj = GameObjectManager::CreateObject ( );
	planetObj->GetTransform ( ).SetScale ( 1.0f );
	planetObj->GetTransform ( ).SetPosition ( 0.0f , 5.0f , 0.0f );

	auto planetMesh = ( MeshRenderer * ) planetObj->AddComponent ( ComponentTypes::MESH_RENDERER );
	planetMesh->SetMaterial ( m_PlanetMaterial );
	planetMesh->loadObjModel ( "ball.obj" );

	auto noise = ( NoiseController * ) planetObj->AddComponent ( ComponentTypes::NOISE_CONTROLLER );
	noise->SetMaterial ( m_PlanetMaterial );
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
		m_FBO->RenderQuad ( m_shaderProgramFBO );
	}

	Renderer::Service ( );
	if ( m_skyBox != nullptr )
	{
		m_skyBox->Draw ( );
	}

	_gameDisplay.swapBuffer ( );
}

