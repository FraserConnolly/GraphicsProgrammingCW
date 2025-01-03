#pragma once

#include <unordered_map>

#include "GameObject.h"

class Material;

class MaterialSwitch : public Component
{
	friend class GameObject;

private:
	MaterialSwitch( GameObject & hostObject );
	~MaterialSwitch ( ) 
	{ }

public:

	void Update ( ) override;
	void AddMaterial ( const int keycode, Material * const material );
	void RemoveMaterial ( const int keycode );
	void ClearMaterials ( );

	void Deserialise ( const json & data ) override;

private:

	std::unordered_map<int, Material *> m_materials;

};

