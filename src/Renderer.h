#pragma once

#include "Mesh.h"

class MeshRenderer;
class Texture;

class Renderer
{

public:

	static void Startup  ( );
	static void Service  ( );
	static void Shutdown ( );

	static void RegisterMeshRenderer   ( const MeshRenderer * pMeshRenderer );
	static void DeregisterMeshRenderer ( const MeshRenderer * pMeshRenderer );

	static const GLint GetMaxTextureUnits ( )
	{
		return s_maxTextureUnit;
	}
	
	static GLint BindTexture ( Texture * pTexture );

private:

	static Renderer * s_instance;
	std::vector<const MeshRenderer *> m_renderers;
	static GLint s_maxTextureUnit;
	
	// An array which will contain a pointer to each of the active textures.
	static Texture ** s_activeTextures;
	static GLint s_lastUsedTextureUnit;

	/// <summary>
	/// Will return the first available free texture unit if there is one.
	/// If all are used then it will return -1.
	/// </summary>
	/// <returns></returns>
	static GLint FindFreeTextureUnit ( );

	// This is the bull in the china shop approach.
	// A more elegant method should be written.
	static void FreeAllTextureUnits ( );

	static bool CheckUnitsAvilable ( unsigned int count );

};

