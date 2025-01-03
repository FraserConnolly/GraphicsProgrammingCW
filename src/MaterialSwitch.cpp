#include "MaterialSwitch.h"
#include "Material.h"
#include "Mesh.h"
#include "Input.h"

MaterialSwitch::MaterialSwitch ( GameObject & hostObject ) :
	Component( hostObject, ComponentTypes::MATERIAL_SWITCHER ), m_materials( )
{ }

void MaterialSwitch::Update ( )
{
	MeshRenderer * meshRenderer = nullptr;
	m_gameObject.GetComponent( ComponentTypes::MESH_RENDERER, meshRenderer );

	if ( meshRenderer == nullptr )
	{
		return;
	}

	for ( auto & pair : m_materials )
	{
		if ( Input::WasKeyPressedThisFrame( pair.first ) )
		{
			meshRenderer->SetMaterial( pair.second );
			return;
		}
	}
}

void MaterialSwitch::AddMaterial ( const int keycode , Material * const material )
{
	Input::RegisterKey( keycode );
	m_materials[ keycode ] = material;
}

void MaterialSwitch::RemoveMaterial ( const int keycode )
{
	m_materials.erase( keycode );
}

void MaterialSwitch::ClearMaterials ( )
{
	m_materials.clear( );
}

void MaterialSwitch::Deserialise ( const json & data )
{
	__debugbreak( );
}
