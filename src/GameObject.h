#pragma once

#include <vector>

#include "Component.h"
#include "Transform.h"

class GameEngine;
class Collider;

/// <summary>
/// The whether an object is active or not is defined by the enabled flag of its transform.
/// </summary>
class GameObject
{ 
public: 
	
	void Update ( );

	void OnCollisionEnter ( const Collider & otherCollider );

	void OnCollisionExit ( const Collider & otherCollider );

	Transform & GetTransform ( ) const
	{
		return m_transform;
	}
	
	const int GetID ( ) const
	{
		return m_id;
	}

	void Destroy ( )
	{
		m_transform.SetActive ( false ); // will call OnDisable on any enabled components
		RemoveAllComponents ( ); // this will also destroy children through the transform component.
		m_isDestroyed = true;
	}

	const bool GetIsDestroyed ( ) const
	{
		return m_isDestroyed;
	}

	const bool IsActive ( ) const
	{
		return m_transform.IsEnabled ( );
	}

	const bool IsActiveInHierarchy ( ) const
	{
		if ( !m_transform.IsEnabled ( ) )
		{
			return false;
		}
		
		// check parents

		if ( m_transform.m_parent == nullptr )
		{
			// no parents
			return true;
		}

		// will call recursively up the hierarchy 
		return m_transform.m_parent->m_gameObject.IsActiveInHierarchy ( );
	}

#pragma region Components

	Component * AddComponent ( ComponentTypes Component );
	void        RemoveComponent ( Component * const pToRemove );
	
	// Kill is called on the component but OnDestroy and delete is not called until the end of the frame
	void        RemoveAllComponents ( );
	void        RemoveAllComponents ( ComponentTypes type );
	
	template<typename T>
	void GetComponent ( ComponentTypes type , T *& pComp )
	{
		size_t size = m_components.size ( );
		for ( size_t i = 0; i < size; ++i )
		{
			//if we found the correct type, set and return 
			if ( m_components [ i ]->GetType ( ) == type )
			{
				pComp = static_cast< T * >( m_components [ i ] );
				return;
			}
		}
		pComp = 0;
	}

	template<typename T>
	void GetAllComponent ( ComponentTypes type , std::vector<T *> & comps )
	{
		size_t size = m_components.size ( );
		for ( size_t i = 0; i < size; ++i )
		{
			//if we found the correct type, add to vector 
			if ( m_components [ i ]->GetType ( ) == type )
				comps.push_back ( static_cast< T * >( m_components [ i ] ) );
		}
	}

private:
	/// <summary>
	/// Called at the end of each frame. Will call OnDestory the delete the component.
	/// </summary>
	void CleanUpComponents ( );

	/// <summary>
	/// Called in Shutdown to immediately remove a component, it will call Kill, OnDestroy, then delete.
	/// </summary>
	void RemoveAllComponentsImmediately ( );

#pragma endregion

private: 
	
	friend class GameObjectManager;
	
	// Transform must be a friend as it needs to call OnEnable / OnDisable on the other components when it is 
	// set to enabled or disabled
	friend class Transform; 
	
	Component * AddComponent ( Component * const pComponent );
	void OnActiveChanged ( bool enabled ); // called only from Transform

	GameObject ( );
	~GameObject ( );

	unsigned int m_id;
	Transform & m_transform;
	
	//Shorter name for my vector 
	typedef std::vector<Component *> ComponentVector;
	//Shorter name for my iterator 
	typedef ComponentVector::iterator  VectorItor;

	ComponentVector m_components;

	bool m_isDestroyed = false;

	static unsigned int  s_objectIDCounter;

	static Component * CreateComponent ( ComponentTypes component, GameObject & hostObject );
};

