#include "Renderer.h"
#include "GameObject.h"
#include "Mesh.h"
#include "Material.h"
#include "Camera.h"
#include "CubeMap.h"

#include <GL\glew.h>

void Renderer::Startup ( )
{
    s_instance = new Renderer ( );
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &s_maxTextureUnit );
    
    // create an array which will contain a pointer to each of the active textures.
    s_activeTextures = new Texture * [ s_maxTextureUnit ];

    s_lastUsedTextureUnit = 0;

    for ( GLint i = 0; i < s_maxTextureUnit; i++ )
    {
        s_activeTextures [ i ] = nullptr;
    }
}

void Renderer::Service ( )
{ 
    for ( auto & camera : s_instance->m_cameras )
    {
        if ( !camera->IsActiveAndEnabled ( ) )
        {
            // this camera should not be rendered
			continue;
        }

        FrameBuffer * fbo = camera->GetFrameBufferObject ( );

        if ( fbo != nullptr )
        {
            fbo->Bind ( );
        }

        RenderObjectsForCamera ( camera );

        if ( camera->GetRenderSkybox ( ) && s_instance->m_skybox != nullptr )
        {
            s_instance->m_skybox->Draw ( *camera );
        }

        if ( fbo != nullptr )
        {
			fbo->Unbind ( );
		}
    }
}

void Renderer::RenderObjectsForCamera ( Camera *& camera )
{
    for ( auto & object : s_instance->m_renderers )
    {
        if ( !object->IsActiveAndEnabled ( ) )
        {
            // this mesh should not be rendered
            continue;
        }

        Material * material = object->GetMaterial ( );

        if ( material == nullptr )
        {
            // there isn't a shader for this material
            // arguably it should still be drawn but with a default material
            continue;
        }

        Shader & shader = material->GetShader ( );

        // bind and update the shader
        shader.Bind ( );
        shader.Update ( *camera , object->GetGameObject ( ).GetTransform ( ) );

        // count the number of unbound textures

        int textureCount = 0;

        for ( auto & pair : material->m_textures )
        {
            Texture * texture = pair.second;
            if ( texture->_activeBind < 0 )
            {
                textureCount++;
            }
        }

        if ( textureCount >= s_maxTextureUnit )
        {
            // this object can not be rendered as it requires more textures than the GPU supports.
            // should be logged.
            continue;
        }

        if ( !CheckUnitsAvilable ( textureCount ) )
        {
            // we need more texture units
            FreeAllTextureUnits ( );
        }

        // the pair here is a GLint representing the uniform location that this texture should be applied to and the texture object to apply.
        for ( auto & pair : material->m_textures )
        {
            Texture * texture = pair.second;

            BindTexture ( texture );

            shader.SetUniform ( pair.first , texture->_activeBind );
        }

        // the pair here is a GLint representing the uniform location that this value that should be set to it.
        for ( auto & pair : material->m_floats )
        {
            float value = pair.second;

            shader.SetUniform ( pair.first , value );
        }

        // the pair here is a GLint representing the uniform location that this value that should be set to it.
        for ( auto & pair : material->m_float3s )
        {
            glm::vec3 value = pair.second;

            shader.SetUniform ( pair.first , value.x , value.y , value.z );
        }

        // draw the mesh
        object->Draw ( );
    }
}

void Renderer::Shutdown ( )
{
    // to do, this deletion is not ideal.
    delete s_instance->m_skybox;
    delete s_instance;
    delete[ ] s_activeTextures;
}

void Renderer::RegisterMeshRenderer ( const MeshRenderer * pMeshRenderer )
{ 
    if ( pMeshRenderer == nullptr )
    {
        // to do - log this error.
        return;
    }

    //Make sure this component doesn't already exist in the list
    auto found = std::find ( s_instance->m_renderers.begin ( ),
                             s_instance->m_renderers.end ( ), pMeshRenderer );

    if ( found != s_instance->m_renderers.end ( ) )
    {
        return;
    }

    s_instance->m_renderers.push_back ( pMeshRenderer );
}

void Renderer::DeregisterMeshRenderer ( const MeshRenderer * pMeshRenderer )
{ 
    auto end = s_instance->m_renderers.end ( );
    auto itor = std::find ( s_instance->m_renderers.begin ( ), end, pMeshRenderer );

    if ( itor == end )
        return;

    s_instance->m_renderers.erase ( itor );
}

void Renderer::RegisterCamera ( Camera * pCamera )
{
    if ( pCamera == nullptr )
    {
        // to do - log this error.
        return;
    }

    //Make sure this component doesn't already exist in the list
    auto found = std::find ( s_instance->m_cameras.begin ( ) ,
        s_instance->m_cameras.end ( ) , pCamera );

    if ( found != s_instance->m_cameras.end ( ) )
    {
        return;
    }

    s_instance->m_cameras.push_back ( pCamera );
}

void Renderer::DeregisterCamera ( Camera * pCamera )
{
    auto end = s_instance->m_cameras.end ( );
    auto itor = std::find ( s_instance->m_cameras.begin ( ) , end , pCamera );

    if ( itor == end )
        return;

    s_instance->m_cameras.erase ( itor );
}

void Renderer::SetSkybox ( CubeMap * pCubeMap )
{
    s_instance->m_skybox = pCubeMap;
}

GLint Renderer::FindFreeTextureUnit ( )
{
    auto tuToTest = s_lastUsedTextureUnit;

    for ( ; ; )
    {
        if ( s_activeTextures [ tuToTest ] == nullptr )
        {
            return tuToTest;
        }

        tuToTest++;

        if ( tuToTest >= s_maxTextureUnit )
        {
            tuToTest = 0;
        }

        if ( tuToTest == s_lastUsedTextureUnit )
        {
            // we've tested all units there are none that are free.
            return -1;
        }
    }
}

void Renderer::FreeAllTextureUnits ( )
{
    for ( GLint i = 0; i < s_maxTextureUnit; i++ )
    {
        Texture * texture = s_activeTextures [ i ];
        
        if ( texture == nullptr )
        {
            continue;
        }

        texture->_activeBind = -1;
        s_activeTextures [ i ] = nullptr;
    }
    
    s_lastUsedTextureUnit = 0;
}

GLint Renderer::BindTexture ( Texture * pTexture )
{
    // is the texture already bound to a Texture Unit
    if ( pTexture->_activeBind >= 0 )
    {
        // confirm the cached activeBind is still true
        if ( s_activeTextures [ pTexture->_activeBind ] == pTexture )
        {
            // texture is already active
            return pTexture->_activeBind;
        }
    }

    auto tu = FindFreeTextureUnit ( );

    if ( tu == -1 )
    {
        // this is a fatal error and should be logged.
        return -1;
    }
    
    s_activeTextures [ tu ] = pTexture;
    pTexture->Bind ( tu );
    return tu;
}

bool Renderer::CheckUnitsAvilable ( unsigned int count )
{
    for ( GLint i = 0; i < s_maxTextureUnit; i++ )
    {
        if ( count == 0 )
        {
            return true;
        }

        if ( s_activeTextures [ i ] == nullptr )
        {
            // this texture unit is available so decrement the amount of units still needed.
            count--;
        }
    }

    return false;
}


Texture ** Renderer::s_activeTextures = nullptr;
GLint Renderer::s_lastUsedTextureUnit = 0;
GLint Renderer::s_maxTextureUnit = 32;
Renderer * Renderer::s_instance = nullptr;
