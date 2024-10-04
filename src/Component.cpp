#include "Component.h"
#include "GameObject.h"

Component::Component ( GameObject & hostObject, const ComponentTypes type ) :
	Component ( hostObject, true, type )
{ }

Component::Component ( GameObject & hostObject, const bool enable, const ComponentTypes type ) :
	m_componentType ( type ), 
	m_gameObject ( hostObject ), 
	m_transform ( hostObject.GetTransform ( ) ),
	m_isEnabled ( enable )
{ }

GameObject & Component::GetGameObject ( ) const
{
	return m_gameObject;
}

bool Component::IsDead ( ) const
{
	return m_isDead;
}

void Component::Kill ( )
{ 
	SetActive ( false );
	m_isDead = true;
}

const bool Component::IsEnabled ( ) const
{
	return m_isEnabled;
}

const bool Component::IsActiveAndEnabled ( ) const
{
	if ( ! m_isEnabled )
	{
		return false;
	}

	return m_gameObject.IsActiveInHierarchy ( );
}

void Component::SetActive ( const bool enable )
{
	if ( m_isEnabled == enable )
	{
		return;
	}

	m_isEnabled = enable;

	if ( enable )
	{
		OnEnable();
	}
	else
	{
		OnDisable ( );
	}

}
