#pragma once

#include "Mesh.h"

class MeshRenderer;
class Texture;
class CubeMap;
class Shader;
class Material;
class Camera;
class FrameBuffer;

class Renderer
{

public:

	static void Startup  ( );
	static void Service  ( );
	static void Shutdown ( );

	static void RegisterTexture    ( const std::string & name , Texture * pTexture );
	static void DeregisterTexture ( const Texture * pTexture );
	static Texture * GetTexture ( const std::string & name );

	static void RegisterShader    ( const std::string & name , Shader * pShader );
	static void DeregisterShader  ( const Shader * pShader );
	static Shader * GetShader ( const std::string & name );

	static void RegisterMaterial    ( const std::string & name , Material * pMaterial );
	static void DeregisterMaterial  ( const Material * pMaterial );
	static Material * GetMaterial ( const std::string & name );

	static void RegisterFrameBuffer ( const std::string & name , FrameBuffer * pFrameBuffer );
	static void DeregisterFrameBuffer ( const FrameBuffer * pFrameBuffer );
	static FrameBuffer * GetFrameBuffer ( const std::string & name );

	static void RegisterMeshRenderer   ( const MeshRenderer * pMeshRenderer );
	static void DeregisterMeshRenderer ( const MeshRenderer * pMeshRenderer );

	static void RegisterCamera   ( Camera * pCamera );
	static void DeregisterCamera ( Camera * pCamera );

	// This will set the cube map to be used by the renderer.
	// This is a known limiting factor, all cameras must either use this cube map or none at all.
	// In future this could be moved to the camera class.
	static void SetSkybox ( CubeMap * pCubeMap );

	static const GLint GetMaxTextureUnits ( )
	{
		return s_maxTextureUnit;
	}
	
	static GLint BindTexture ( Texture * pTexture );

private:

	static Renderer * s_instance;
	std::vector<const MeshRenderer *> m_renderers;
	std::map<std::string , Texture *> m_textures;
	std::map<std::string , Shader *> m_shaders;
	std::map<std::string , Material *> m_materials;
	std::map<std::string , FrameBuffer *> m_fbos;
	std::vector<Camera *> m_cameras; // can't be const because of the cache on getting the view matrix.
	CubeMap * m_skybox = nullptr;
	static GLint s_maxTextureUnit;
	
	// An array which will contain a pointer to each of the active textures.
	static Texture ** s_activeTextures;
	static GLint s_lastUsedTextureUnit;

	static void RenderObjectsForCamera ( Camera *& camera );

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

