#pragma once
#include <string>
#include <GL\glew.h>
#include "Transform.h"
#include "Camera.h"

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
	
	void LoadDefaultShaders ( );
	void LoadDefaultGeometoryShaders( );
	void LoadShaders ( const std::string& vertShader, const std::string& fragShader );
	void LoadShaders ( const std::string & vertShader, const std::string& geoShader, const std::string & fragShader );
	void LoadShaders ( const char * vertexShader, const char * geoShader, const char * fargmentShader );
	
	void SetCamera ( Camera * const camera )
	{
		_camera = camera;
	}

	void Bind ( ) const; //Set GPU to use our shaders

	std::string LoadShader ( const std::string & fileName );
	
	void CheckShaderError ( GLuint shader, GLuint flag, bool isProgram, const std::string &errorMessage );
	
	GLuint CreateShader ( const std::string & text, GLenum type );
	
	/// <summary>
	/// Ensure that the shader program is being used before applying uniforms.
	/// </summary>
	void SetUniform ( const GLchar * name, const GLboolean v ) const;
	void SetUniform ( const GLchar * name, const GLint v ) const;
	void SetUniform ( const GLint location, const GLint v ) const;
	void SetUniform ( const GLint location, const float value ) const;
	void SetUniform ( const GLint location, const float x, const float y, const float z ) const;
	void SetUniform ( const GLchar * name, const GLfloat v ) const;
	void SetUniform ( const GLchar * name, const GLfloat x, const GLfloat y, const GLfloat z ) const;
	void SetUniform ( const GLchar * name, const GLfloat x, const GLfloat y, const GLfloat z, const GLfloat w ) const;
	void SetUniform ( const GLchar * name, const glm::mat4 & matrix );
	void SetTransform ( const glm::mat4 & transform );
	GLint GetUniformLocation ( const GLchar * name ) const;

	void Update ( Transform & transform );

private:
	
	static const unsigned int MAX_SHADER_COUNT = 3; // number of shaders
	
	enum
	{
		MODEL_U,
		VIEW_U,
		PROJECTION_U,
		TIME_U,
		FOG_COLOUR,
		FOG_MIN_DIST,
		FOG_MAX_DIST,

#ifdef USE_ADS
		LIGHT_POSITION_U,
		LIGHT_COLOUR_U,
		OBJECT_COLOUR_U,
#endif

		NUM_UNIFORMS
	};

	Shader ( const Shader & other )
		:_program( 0 ), _shaders { 0, 0, 0 }, _uniforms{ 0 }, _numShaders( 0 )
	{ }
	
	void operator=( const Shader & other )
	{ }
	
	int _numShaders;
	GLuint _program; // Track the shader program
	GLuint _shaders [ MAX_SHADER_COUNT ]; //array of shaders
	GLuint _uniforms [ NUM_UNIFORMS ];

	Camera * _camera = nullptr;

};