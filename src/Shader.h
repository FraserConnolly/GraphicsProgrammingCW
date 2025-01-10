#pragma once
#include <string>
#include <GL\glew.h>
#include "Transform.h"

class Camera;
class DirectionalLight;

class Shader
{
public:

	// These match the locations of the attributes in the vertex shader.
	static const int SHADER_ATTRIBUTE_INDEX_POSITION           = 0;
	static const int SHADER_ATTRIBUTE_INDEX_NORMAL             = 1;
	static const int SHADER_ATTRIBUTE_INDEX_COLOUR             = 2;
	static const int SHADER_ATTRIBUTE_INDEX_TEXTURE_COORDINATE = 3;

	Shader ( );
	~Shader ( );
	
	void LoadShaders ( const std::string& vertShader, const std::string& fragShader );
	void LoadShaders ( const std::string & vertShader, const std::string& geoShader, const std::string & fragShader );
	void LoadShaders ( const char * vertexShader, const char * geoShader, const char * fargmentShader );
	
	void Bind ( ) const; //Set GPU to use our shaders

	std::string LoadShader ( const std::string & fileName );
	
	void CheckShaderError ( GLuint shader, GLuint flag, bool isProgram, const std::string &errorMessage );
	
	GLuint CreateShader ( const std::string & text, GLenum type );
	
	/// <summary>
	/// Ensure that the shader program is being used before applying uniforms.
	/// </summary>
	void SetUniform ( const GLint location, const GLint v ) const;
	void SetUniform ( const GLint location , const float value ) const;
	void SetUniform ( const GLint location , const float x , const float y , const float z ) const;
	void SetUniform ( const GLint location , const float x , const float y , const float z, const float w ) const;
	void SetUniform ( const GLint location , const GLboolean v ) const;
	void SetUniform ( const GLint location , const glm::mat4 & matrix ) const;

	void SetUniformByName ( const GLchar * name , const GLint v ) const;
	void SetUniformByName ( const GLchar * name , const GLfloat v ) const;
	void SetUniformByName ( const GLchar * name , const GLfloat x , const GLfloat y , const GLfloat z ) const;
	void SetUniformByName ( const GLchar * name , const GLfloat x , const GLfloat y , const GLfloat z , const GLfloat w ) const;
	void SetUniformByName ( const GLchar * name , const GLboolean v ) const;
	void SetUniformByName ( const GLchar * name, const glm::mat4 & matrix ) const;
	
	// Make sure the texture is bound and shadow map has been rendered before calling this function
	void SetDirectionalLight ( const DirectionalLight & light ) const;

	GLint GetUniformLocation ( const GLchar * name ) const;

	void Update ( Camera & camera, Transform & transform );

private:
	
	static const unsigned int MAX_SHADER_COUNT = 3; // number of shaders
	
	enum
	{
		MODEL_U,
		VIEW_U,
		PROJECTION_U,
		TIME_U,

		CAMERA_POS_U,

		FOG_COLOUR_U,
		FOG_MIN_DIST_U,
		FOG_MAX_DIST_U,

		//Lighting
		LIGHT_SPACE_MATRIX_U,
		SHADOW_MAP_U,

		LIGHT_POSITION_U,
		LIGHT_COLOUR_U,

		NUM_UNIFORMS
	};

	Shader ( const Shader & other )
		:_program( 0 ), _shaders { 0, 0, 0 }, _uniforms{ 0 }, _numShaders( 0 )
	{ }
	
	void operator=( const Shader & other )
	{ }
	
	unsigned int _numShaders;
	GLuint _program; // Track the shader program
	GLuint _shaders [ MAX_SHADER_COUNT ]; //array of shaders
	GLint  _uniforms [ NUM_UNIFORMS ];

};