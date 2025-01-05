#include "MaterialSwitch.h"
#include "Material.h"
#include "Mesh.h"
#include "Input.h"
#include "Renderer.h"

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
	if ( data.contains ( "Materials" ) && data [ "Materials" ].is_array( ) )
	{

		for ( auto & material : data [ "Materials" ] )
		{
			if ( !material.contains ( "Keycode" ) || !material.contains ( "Material" ) )
			{
				// log error
				continue;
			}

			int keycode = material [ "Keycode" ].get<int> ( );
			std::string materialName = material [ "Material" ].get<std::string> ( );

			Material * material = Renderer::GetMaterial ( materialName );

			if ( material == nullptr )
			{
				// log error
				continue;
			}

			AddMaterial ( keycode , material );
		}
	}
}
