#pragma once

#include "ComponentTypes.h"

class GameObject;
class Transform;
class Collider;

#include "json/json.hpp"
using json = nlohmann::json;

class Component
{
public: 
	
	friend class GameObject;

	virtual void Update ( ) { }
	virtual void LateUpdate ( ) { }
	
	virtual void OnCollisionEnter ( const Collider & otherCollider ) { }
	virtual void OnCollisionExit ( const Collider & otherCollider ) { }

	virtual void OnEnable ( ) { }
	virtual void OnDisable ( ) { }

	virtual void Awake ( ) { }
	virtual void Start ( ) { }
	virtual void OnDestroy ( ) { }

	virtual void Reset ( ) { }

	GameObject & GetGameObject ( ) const;

	bool IsDead ( ) const;
	void Kill ( );

	const bool IsEnabled ( ) const;
	const bool IsActiveAndEnabled ( ) const;

	void SetActive ( const bool enable );

	ComponentTypes GetType ( ) const
	{
		return m_componentType;
	}

	virtual void Deserialise ( const json & data ) = 0;

protected:

	bool m_isEnabled = true;
	bool m_isDead = false;
	ComponentTypes m_componentType;

	GameObject & m_gameObject;
	Transform & m_transform;

	Component ( GameObject & hostObject, const ComponentTypes type );
	Component ( GameObject & hostObject, const bool enable, const ComponentTypes type );

private:
	Component ( ) = delete;	
};

