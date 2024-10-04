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

	void SetTexture ( const GLchar * const name, Texture * const pTexture )
	{
		// get the uniform location of the name in the shader program

		m_pShaderProgram.Bind ( );
		auto location = m_pShaderProgram.GetUnifromLocation ( name );

		if ( location >= 0 )
		{
			m_textures [ location ] = pTexture;
		}
		else
		{
			// log a fault.
		}
	}


private:

	Shader & m_pShaderProgram;
	std::unordered_map<GLint, Texture * > m_textures;

};

