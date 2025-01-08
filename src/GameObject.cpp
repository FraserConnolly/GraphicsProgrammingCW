#include <sstream>

#include "GameObject.h"
#include "Camera.h"
#include "Mesh.h"
#include "CameraFlyController.h"
#include "Rotator.h"
#include "Audio Event Emitter.h"
#include "Audio Listener.h"
#include "Path Follow.h"
#include "ColliderBase.h"
#include "ColliderBox.h"
#include "ColliderSphere.h"
#include "PlayerController.h"
#include "ExplosionController.h"
#include "MaterialSwitch.h"
#include "NoiseController.h"
#include "LookAt.h"
#include "TransformFollow.h"

unsigned int GameObject::s_objectIDCounter = 0;

GameObject::GameObject ( )
    : m_transform ( *new Transform ( *this ) ), m_id ( s_objectIDCounter++ )
{
    // assign a default name for this object
    std::ostringstream oss;
    oss << "Object " << m_id;
    m_name = oss.str ( );

    AddComponent ( &m_transform );
}

GameObject::~GameObject ( )
{
    m_transform.SetActive ( false );
	RemoveAllComponentsImmediately ( );
    delete &m_transform;
}

Component * GameObject::CreateComponent ( ComponentTypes component, GameObject & hostObject )
{
    switch ( component )
    {
        case TRANSFORM:
            // Transform can only be constructed inside the initialisation list of a GameObject.
            return nullptr;
        case MESH_RENDERER:
            return new MeshRenderer ( hostObject );
        case BOX_COLIDER:
            return new ColliderBox ( hostObject );
        case SPHERE_COLIDER:
            return new ColliderSphere ( hostObject );
        case CAMERA:
            return new Camera ( hostObject );
        case CAMERA_FLY_CONTROLLER:
            return new CameraFlyController ( hostObject );
        case ROTATOR:
            return new Rotator ( hostObject );
        case AUDIO_EVENT_EMITTER:
            return new AudioEventEmitter ( hostObject );
        case AUDIO_LISTENER:
            return new AudioListener ( hostObject );
        case PATH_FOLLOW:
            return new PathFollow ( hostObject );
        case PLAYER_CONTROLLER:
            return new PlayerController ( hostObject );
        case EXPLOSION_CONTROLLER:
			return new ExplosionController ( hostObject );
        case MATERIAL_SWITCHER:
            return new MaterialSwitch ( hostObject );
        case NOISE_CONTROLLER:
			return new NoiseController ( hostObject );
        case LOOK_AT:
			return new LookAt ( hostObject );
        case TRANSFORM_FOLLOW:
            return new TransformFollow ( hostObject );
    }

    return nullptr;
}

void GameObject::Update ( )
{
    for ( auto & comp : m_components )
    {
        if ( comp->IsEnabled ( ) )
        {
            comp->Update ( );
        }
    }
}

void GameObject::OnCollisionEnter ( const Collider & otherCollider )
{
    // Belt and braces, this shouldn't be needed as the collision manager shouldn't check collisions with disabled colliders.
    if ( !IsActiveInHierarchy ( ) )
    {
        return;
    }

    for ( auto & comp : m_components )
    {
        if ( comp->IsEnabled ( ) )
        {
            comp->OnCollisionEnter ( otherCollider );
        }
    }
}

void GameObject::OnCollisionExit ( const Collider & otherCollider )
{
    // Belt and braces, this shouldn't be needed as the collision manager shouldn't check collisions with disabled colliders.
    if ( !IsActiveInHierarchy ( ) )
    {
        return;
    }

    for ( auto & comp : m_components )
    {
        if ( comp->IsEnabled ( ) )
        {
            comp->OnCollisionExit ( otherCollider );
        }
    }
}

/// <summary>
/// Adds a new component based on the strings used in the FGame file.
/// </summary>
/// <param name="component"></param>
/// <returns></returns>
Component * GameObject::AddComponent ( const std::string component )
{
    ComponentTypes type = ComponentTypes::NOT_FOUND;

    if ( component == "MeshRenderer" )
        type = MESH_RENDERER;
    else if ( component == "BoxCollider" )
        type = BOX_COLIDER;
    else if ( component == "SphereCollider" )
        type = SPHERE_COLIDER;
    else if ( component == "Camera" )
        type = CAMERA;
    else if ( component == "CameraFlyController" )
        type = CAMERA_FLY_CONTROLLER;
    else if ( component == "Rotator" )
        type = ROTATOR;
    else if ( component == "AudioEventEmitter" )
        type = AUDIO_EVENT_EMITTER;
    else if ( component == "AudioListener" )
        type = AUDIO_LISTENER;
    else if ( component == "PathFollow" )
        type = PATH_FOLLOW;
    else if ( component == "PlayerController" )
        type = PLAYER_CONTROLLER;
    else if ( component == "ExplosionController" )
        type = EXPLOSION_CONTROLLER;
    else if ( component == "MaterialSwitch" )
        type = MATERIAL_SWITCHER;
    else if ( component == "NoiseController" )
        type = NOISE_CONTROLLER;
    else if ( component == "LookAt" )
        type = LOOK_AT;
    else if ( component == "TransformFollow" )
		type = TRANSFORM_FOLLOW;

    if ( type == ComponentTypes::NOT_FOUND )
    {
        // unknown component type requested in the FGame file.
        return nullptr;
    }

    return AddComponent ( type );
}

Component * GameObject::AddComponent ( ComponentTypes component )
{
    Component * pComponent = CreateComponent ( component , *this );

    if ( pComponent == nullptr )
    {
        return nullptr;
    }

    return AddComponent ( pComponent );
}

Component * GameObject::AddComponent ( Component * const pComponent )
{
    //Make sure this component doesn't already exist in the components list
    VectorItor found = std::find ( m_components.begin ( ),
                                   m_components.end ( ), pComponent );

    if ( found != m_components.end ( ) )
    {
        return pComponent;
    }

    m_components.push_back ( pComponent );

    pComponent->Awake ( );
    if ( pComponent->IsActiveAndEnabled ( ) )
    {
        pComponent->OnEnable ( );
    }

    return pComponent;
}

void GameObject::OnActiveChanged ( bool enabled )
{
    for ( auto & comp : m_components )
    {
        if ( comp->GetType ( ) == TRANSFORM )
        {
            continue;
        }

        if ( !comp->IsEnabled ( ) )
        {
            // component is disabled so ignore it.
            continue;
        }

        if ( enabled )
        {
            comp->OnEnable ( );
        }
        else
        {
            comp->OnDisable ( );
        }
    }
}

void GameObject::RemoveComponent ( Component * const pToRemove )
{ 
    if ( pToRemove->GetType ( ) == TRANSFORM )
    {
        // Can not remove a transform.
        return;
    }

    //Make the sure the instance exists in this object 
    VectorItor end = m_components.end ( );
    VectorItor itor = std::find ( m_components.begin ( ), end, pToRemove );

    if ( itor != end )
        return;

    ( *itor )->Kill ( );

    // note - don't erase or delete in this function, do that in the clean up.
}

void GameObject::RemoveAllComponentsImmediately ( )
{ 
    VectorItor itor = m_components.begin ( );
    VectorItor end = m_components.end ( );
    while ( itor != end )
    {
        ( *itor )->Kill ( ); 
        ( *itor )->OnDestroy ( );

        // let the GameObject's destructor delete Transform.
        if ( ( *itor )->GetType ( ) != TRANSFORM ) 
        {
            delete * itor;
        }
        ++itor;
    }
    m_components.clear ( );
}



void GameObject::CleanUpComponents ( )
{ 
    int endIndex = m_components.size ( ) - 1;
    for ( ; endIndex >= 0; --endIndex )
    {
        if ( m_components [ endIndex ]->IsDead( ) )
        {
            m_components [ endIndex ]->OnDestroy ( );
            delete m_components [ endIndex ];
            m_components [ endIndex ] = m_components [ m_components.size ( ) - 1 ]; // copy the pointer to the last component to this element
            m_components.pop_back ( );  // pop the last element which is now a duplicate.
        }
    }
}


void GameObject::RemoveAllComponents ( )
{
    for ( size_t i = 0; i < m_components.size ( ); ++i )
    {
        if ( m_components[i]->GetType( ) == TRANSFORM )
        {
            // can not remove the transform component.
            continue;
        }
    
        m_components [ i ]->Kill ( );
    }
}

void GameObject::RemoveAllComponents ( ComponentTypes type )
{
    if ( type == TRANSFORM )
    {
        // can not remove the transform component.
        return;
    }

    for ( size_t i = 0; i < m_components.size ( ); ++i )
    {
        if ( m_components [ i ]->GetType ( ) == type )
        {
            m_components [ i ]->Kill ( );
        }
    }
}


