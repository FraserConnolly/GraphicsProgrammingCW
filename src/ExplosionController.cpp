#include "ExplosionController.h"
#include "Time.h"
#include "Material.h"
#include "Renderer.h"

ExplosionController::ExplosionController ( GameObject & hostObject ) : 
	Component( hostObject , ComponentTypes::EXPLOSION_CONTROLLER ), 
	m_uniformTimeLocation( 0 ), 
	m_speed( 1.0f ), 
	m_material( nullptr )
{ } 

void ExplosionController::Update()
{
	if ( m_uniformTimeLocation == 0 ) { return; }

	m_material -> SetFloat ( m_uniformTimeLocation, Time::GetTime ( ) * m_speed );
}

void ExplosionController::SetExplosionAmount ( const float amount )
{
	m_material->SetFloatByName ( "magnitude" , amount );
}

void ExplosionController::SetMaterial ( Material * const material )
{
	if ( material == nullptr )
	{
		m_uniformTimeLocation = 0;
		m_material = nullptr;
		return;
	}

	m_material = material;
	m_uniformTimeLocation = m_material -> GetUniformLocation ( "time" );
}

void ExplosionController::Deserialise ( const json & data )
{
	if ( data.contains ( "Speed" ) )
	{
		SetSpeed ( data [ "Speed" ].get<float> ( ) );
	}

	if ( data.contains ( "Material" ) )
	{
		SetMaterial ( Renderer::GetMaterial ( data [ "Material" ].get<std::string> ( ) ) );
	}

	if ( data.contains ( "ExplosionAmount" ) )
	{
		SetExplosionAmount ( data [ "ExplosionAmount" ].get<float> ( ) );
	}
}
