#include "NoiseController.h"
#include "Time.h"
#include "Material.h"
#include "Renderer.h"

NoiseController::NoiseController ( GameObject & hostObject ) :
	Component( hostObject , ComponentTypes::NOISE_CONTROLLER ), 
	m_uniformTimeLocation( 0 ), 
	m_unifromFogDensity ( 0 ),
	m_unifromFogColour( 0 ),
	m_uniformMaxDist( 0 ),
	m_UnifromMinDist( 0 ),
	m_speed( 1.0f ), 
	m_material( nullptr )
{ } 

void NoiseController::Update()
{
	if ( m_material == nullptr ) { return; }

	m_material -> SetFloat ( m_uniformTimeLocation, Time::GetTime ( ) * m_speed );
}

void NoiseController::SetSpeed ( const float speed )
{
	m_speed = speed;
}

const float NoiseController::GetSpeed ( ) const
{
	return m_speed;
}

void NoiseController::SetMaterial ( Material * const material )
{
	if ( material == nullptr )
	{
		m_uniformTimeLocation = 0;
		m_material = nullptr;
		return;
	}

	m_material = material;
	m_uniformTimeLocation = m_material -> GetUniformLocation ( "time" );
	m_unifromFogDensity = m_material -> GetUniformLocation ( "fogDensity" );
	m_unifromFogColour = m_material->GetUniformLocation ( "fogColor" );
	m_uniformMaxDist = m_material->GetUniformLocation ( "maxDist" );
	m_UnifromMinDist = m_material->GetUniformLocation ( "minDist" );

}

void NoiseController::Deserialise ( const json & data )
{
	if ( data.contains ( "Speed" ) )
	{
		SetSpeed ( data [ "Speed" ].get<float> ( ) );
	}

	if ( data.contains ( "Material" ) )
	{
		auto & material = data [ "Material" ];
		if ( material.is_string ( ) )
		{
			SetMaterial ( Renderer::GetMaterial ( material.get<std::string> ( ) ) );
		}
	}
}
