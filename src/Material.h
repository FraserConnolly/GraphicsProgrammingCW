#pragma once

#include <unordered_map>

#include "Shader.h"
#include "Texture.h"

/*
* Each MeshRenderer contains 1 or more materials.
* Each material must and shader program associated with it and 0 or more textures.
*/
class Material
{ 
	friend class Renderer;

public:

	Material ( Shader * pShaderProgram ) : 
		m_pShaderProgram ( *pShaderProgram )
	{
	}

	Shader & GetShader ( ) const
	{
		return m_pShaderProgram;
	}

	GLint GetUniformLocation ( const GLchar * const name )
	{
		// get the uniform location of the name in the shader program
		m_pShaderProgram.Bind( );
		return m_pShaderProgram.GetUniformLocation ( name );
	}

	void SetTexture ( const GLchar * const name, Texture * const pTexture )
	{
		auto location = GetUniformLocation ( name );
		SetTexture ( location, pTexture );
	}

	void SetTexture ( const GLint location, Texture * const pTexture )
	{
		if ( location >= 0 )
		{
			m_textures [ location ] = pTexture;
		}
		else
		{
			// log a fault.
		}
	}

	void SetFloat(const GLchar* const name, const float const value )
	{
		auto location = GetUniformLocation ( name );
		SetFloat ( location , value );
	}

	void SetFloat ( const GLint location , const float const value )
	{
		if ( location >= 0 )
		{
			m_floats [ location ] = value;
		}
		else
		{
			// log a fault.
		}
	}

	void SetFloat3 ( const GLchar * const name , glm::vec3 & value  )
	{
		SetFloat3 ( name , value.x , value.y , value.z );
	}

	void SetFloat3 ( const GLint location , glm::vec3 & value )
	{
		SetFloat3 ( location , value.x , value.y , value.z );
	}

	void SetFloat3(const GLchar* const name, const float const x, const float const y, const float const z)
	{
		auto location = GetUniformLocation ( name );
		SetFloat3 ( location , x, y, z );
	}

	void SetFloat3 ( const GLint location , const float const x , const float const y , const float const z )
	{
		if ( location >= 0 )
		{
			m_float3s [ location ] = glm::vec3 ( x , y , z );
		}
		else
		{
			// log a fault.
		}
	}


private:

	Shader & m_pShaderProgram;
	std::unordered_map<GLint, Texture *> m_textures;
	std::unordered_map<GLint, float> m_floats;
	std::unordered_map<GLint, glm::vec3> m_float3s;

};

