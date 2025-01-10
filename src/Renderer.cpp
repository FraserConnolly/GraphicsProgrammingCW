#include "Renderer.h"
#include "GameObject.h"
#include "Mesh.h"
#include "Material.h"
#include "Camera.h"
#include "Texture.h"
#include "Shader.h"
#include "CubeMap.h"
#include "DirectionalLight.h"

#include <GL\glew.h>
#include <map>
#include <vector>

void Renderer::Startup ( const int width , const int height , const std::string title )
{
    s_instance = new Renderer ( );
    s_instance->m_gameDisplay.initDisplay ( width , height , title );

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
    s_instance->m_gameDisplay.clearDisplay ( );

    // 1st pass to render the scene from the perspective of the light source
    if ( s_instance->m_directionalLight != nullptr )
	{
        s_instance->m_directionalLight->Bind ( );
        RenderObjectsForDepthMap ( );
        s_instance->m_directionalLight->Unbind ( );
	}

    // 2nd pass to render the scene with shadow mapping
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
            // this will set the viewport to the size of the texture for the frame buffer
            fbo->Bind ( );
        }
        else
        {
            // reset the viewport to the screen size
            glViewport ( 0 , 0 , GetScreenWidth ( ) , GetScreenHeight ( ) );
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

    s_instance->m_gameDisplay.swapBuffer ( );
}

void Renderer::RenderObjectsForDepthMap ( )
{
    for ( auto & object : s_instance->m_renderers )
    {
        if ( !object->IsActiveAndEnabled ( ) )
        {
            // this mesh should not be rendered
            continue;
        }

        if ( ! object->GetCastShadows ( ) )
		{
			// this mesh should not cast shadows
			continue;
		}

        s_instance->m_directionalLight->SetModelInShader (
            object->GetGameObject().GetTransform ( ).GetModel ( ) );

        // draw the mesh
        object->Draw ( );
    }
}

void Renderer::RenderObjectsForCamera ( Camera *& camera )
{
    Texture * shadowMapTexture = nullptr;

    if ( s_instance->m_directionalLight != nullptr )
    {
        shadowMapTexture = s_instance->m_directionalLight->GetShadowMap ( );
        BindTexture ( shadowMapTexture );
    }

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

        if ( shadowMapTexture != nullptr )
        {
            BindTexture ( shadowMapTexture );
            shader.SetDirectionalLight ( * s_instance->m_directionalLight );
        }

        // draw the mesh
        object->Draw ( );
    }
}

void Renderer::Shutdown ( )
{
    for ( auto & frameBuffer : s_instance->m_fbos)
    {
        delete frameBuffer.second;
    }
    s_instance->m_fbos.clear ( );

    for ( auto & material : s_instance->m_materials )
    {
    	delete material.second;
    }
    s_instance->m_materials.clear ( );

    for ( auto & texture : s_instance->m_textures )
    {
    	delete texture.second;
    }
    s_instance->m_textures.clear ( );

    for ( auto & shader : s_instance->m_shaders )
    {
    	delete shader.second;
    }
    s_instance->m_shaders.clear ( );

    // to do, this deletion is not ideal.
    delete s_instance->m_skybox;

    delete s_instance;
    delete[ ] s_activeTextures;

}

void Renderer::RegisterTexture ( const std::string & name , Texture * pTexture )
{
    if ( pTexture == nullptr )
    {
		// to do - log this error.
		return;
	}

    s_instance->m_textures [ name ] = pTexture;
}

void Renderer::DeregisterTexture ( const Texture * pTexture )
{
    if ( pTexture == nullptr )
    {
		// to do - log this error.
		return;
	}

    for ( auto itor = s_instance->m_textures.begin ( ); itor != s_instance->m_textures.end ( ); itor++ )
    {
        if ( itor->second == pTexture )
        {
            s_instance->m_textures.erase ( itor );
			return;
		}
	}
}

Texture * Renderer::GetTexture ( const std::string & name )
{
    auto t = s_instance->m_textures.find( name );
    
    if ( t == s_instance->m_textures.end ( ) )
    {
        return nullptr;
    }

    return t->second;
}

void Renderer::RegisterShader ( const std::string & name , Shader * pShader )
{
    if ( pShader == nullptr )
    {
		// to do - log this error.
		return;
	}

    s_instance->m_shaders [ name ] = pShader;
}

void Renderer::DeregisterShader ( const Shader * pShader )
{
    if ( pShader == nullptr )
    {
		// to do - log this error.
		return;
	}

    for ( auto itor = s_instance->m_shaders.begin ( ); itor != s_instance->m_shaders.end ( ); itor++ )
    {
        if ( itor->second == pShader )
        {
            s_instance->m_shaders.erase ( itor );
			return;
		}
	}
}

Shader * Renderer::GetShader ( const std::string & name )
{
	auto s = s_instance->m_shaders.find ( name );

    if ( s == s_instance->m_shaders.end ( ) )
    {
		return nullptr;
	}

	return s->second;
}

void Renderer::RegisterMaterial ( const std::string & name , Material * pMaterial )
{
    if ( pMaterial == nullptr )
    {
		// to do - log this error.
		return;
	}

    s_instance->m_materials [ name ] = pMaterial;
}

void Renderer::DeregisterMaterial ( const Material * pMaterial )
{
    if ( pMaterial == nullptr )
    {
		// to do - log this error.
		return;
	}

    for ( auto itor = s_instance->m_materials.begin ( ); itor != s_instance->m_materials.end ( ); itor++ )
    {
        if ( itor->second == pMaterial )
        {
            s_instance->m_materials.erase ( itor );
			return;
		}
	}
}

Material * Renderer::GetMaterial ( const std::string & name )
{
    auto m = s_instance->m_materials.find ( name );

    if ( m == s_instance->m_materials.end ( ) )
    {
		return nullptr;
	}

	return m->second;
}

void Renderer::RegisterFrameBuffer ( const std::string & name , FrameBuffer * pFrameBuffer )
{
    if ( pFrameBuffer == nullptr )
    {
        // to do - log this error.
        return;
    }

    s_instance->m_fbos [ name ] = pFrameBuffer;
}

void Renderer::DeregisterFrameBuffer ( const FrameBuffer * pFrameBuffer )
{
    if ( pFrameBuffer == nullptr )
    {
        // to do - log this error.
        return;
    }

    for ( auto itor = s_instance->m_fbos.begin ( ); itor != s_instance->m_fbos.end ( ); itor++ )
    {
        if ( itor->second == pFrameBuffer )
        {
            s_instance->m_fbos.erase ( itor );
            return;
        }
    }
}

FrameBuffer * Renderer::GetFrameBuffer ( const std::string & name )
{
    auto m = s_instance->m_fbos.find ( name );

    if ( m == s_instance->m_fbos.end ( ) )
    {
        return nullptr;
    }

    return m->second;
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

void Renderer::SetDirectionalLight ( DirectionalLight * pDirectionalLight )
{
    s_instance->m_directionalLight = pDirectionalLight;
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
