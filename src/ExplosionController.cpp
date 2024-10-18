#include "ExplosionController.h"
#include "Time.h"
#include "Material.h"

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

void ExplosionController::SetSpeed ( const float speed )
{
	m_speed = speed;
}

const float ExplosionController::GetSpeed ( ) const
{
	return m_speed;
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
