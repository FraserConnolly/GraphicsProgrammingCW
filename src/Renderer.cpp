#include "Renderer.h"
#include "GameObject.h"
#include "Mesh.h"
#include "Material.h"
#include "Time.h"

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
    for ( auto & object : s_instance->m_renderers )
    {
        if ( ! object->IsActiveAndEnabled( ) )
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
        shader.Update ( object->GetGameObject ( ).GetTransform ( ), Time::GetTime( ) );

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

        if ( ! CheckUnitsAvilable ( textureCount ) )
        {
            // we need more texture units
            FreeAllTextureUnits ( );
        }

        // the pair here is a GLint representing the uniform location that this texture should be applied to and the texture object to apply.
        for ( auto & pair : material->m_textures )
        {
            Texture * texture = pair.second;

            BindTexture ( texture );

            shader.SetUniform ( pair.first, texture->_activeBind );
        }

        // draw the mesh
        object->Draw ( );
    }
}

void Renderer::Shutdown ( )
{
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
