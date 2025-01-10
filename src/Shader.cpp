#include "Shader.h"
#include <iostream>
#include <fstream>

#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "GameObject.h"
#include "DirectionalLight.h"
#include "Texture.h"

#ifdef USE_ADS
#define VertexShaderPath "ads.vert"
#define FragmentShaderPath "ads.frag"
#else
#define VertexShaderPath "shader.vert"
#define FragmentShaderPath "shader.frag"
#endif 

#define VertexGeoShaderPath "shaderGeoText.vert"
#define GeometryShaderPath "shaderGeoText.geom"
#define FragmentGeoShaderPath "shaderGeoText.frag"

Shader::Shader ( )
	: _program( 0 ), _shaders( ), _uniforms( ), _numShaders( 0 )
{ 

}

Shader::~Shader ( )
{ 
	// detach and delete our shaders from the program
	for ( size_t i = 0; i < _numShaders; i++ )
	{
		glDetachShader ( _program, _shaders [ i ] ); //detach shader from program
		glDeleteShader ( _shaders [ i ] ); //delete the shaders
	}
	
	glDeleteProgram ( _program );
}

void Shader::LoadDefaultShaders ( )
{
	LoadShaders ( VertexShaderPath, FragmentShaderPath );
}

void Shader::LoadDefaultGeometoryShaders() {
	LoadShaders ( VertexGeoShaderPath, GeometryShaderPath, FragmentGeoShaderPath );
}

void Shader::LoadShaders ( const std::string & vertShader, const std::string & fragShader )
{
	LoadShaders ( vertShader.c_str ( ), NULL, fragShader.c_str ( ) );
}

void Shader::LoadShaders(const std::string& vertShader, const std::string& geoShader, const std::string& fragShader)
{
	LoadShaders(vertShader.c_str(), geoShader.c_str(), fragShader.c_str());
}

void Shader::LoadShaders ( const char * vertexShader, const char * geoShader, const char * fargmentShader )
{ 
	_program = glCreateProgram ( );

	_numShaders = 2;

	if ( geoShader != NULL ) 
	{
		_numShaders = 3;
	}

	_shaders [ 0 ] = CreateShader (
		LoadShader ( vertexShader ),
		GL_VERTEX_SHADER );

	_shaders [ 1 ] = CreateShader (
		LoadShader ( fargmentShader ),
		GL_FRAGMENT_SHADER );

	if ( geoShader != NULL )
	{
		_shaders[2] = CreateShader(
			LoadShader(geoShader),
			GL_GEOMETRY_SHADER);
	}

	for ( size_t i = 0; i < _numShaders; i++ )
	{
		glAttachShader ( _program, _shaders [ i ] ); //attach the shader to the program
	}

	glLinkProgram ( _program ); //create executables that will run on the GPU shaders
	CheckShaderError ( _program, GL_LINK_STATUS, true, "Error: Shader program linking failed" ); // check for error

	// Validate the shader
	glValidateProgram ( _program ); //check the entire program is valid
	CheckShaderError ( _program, GL_VALIDATE_STATUS, true, "Error: Shader program not valid" );

	// Get the location of the uniform variables
	_uniforms [ MODEL_U ]		= glGetUniformLocation ( _program, "model" );
	_uniforms [ VIEW_U ]		= glGetUniformLocation ( _program, "view" );
	_uniforms [ PROJECTION_U ]	= glGetUniformLocation ( _program, "projection" );
	_uniforms [ TIME_U ]		= glGetUniformLocation ( _program, "time" );
	
	_uniforms [ CAMERA_POS_U ]	= glGetUniformLocation ( _program, "cameraPos" );

	_uniforms [ FOG_COLOUR_U ]		= glGetUniformLocation( _program, "fogColor" );
	_uniforms [ FOG_MIN_DIST_U ]	= glGetUniformLocation( _program, "maxDist" );
	_uniforms [ FOG_MAX_DIST_U ]	= glGetUniformLocation( _program, "minDist" );

#ifdef USE_ADS
	_uniforms[ LIGHT_POSITION_U ] = glGetUniformLocation( _program, "lightPos" );
	_uniforms[ LIGHT_COLOUR_U ]   = glGetUniformLocation( _program, "lightColor" );
#endif

	_uniforms [ LIGHT_SPACE_MATRIX_U ]	= glGetUniformLocation ( _program, "lightSpaceMatrix" );
	_uniforms [ SHADOW_MAP_U ]			= glGetUniformLocation ( _program, "shadowMap" );

	for ( size_t i = 0; i < _numShaders; i++ )
	{
		glDeleteShader ( _shaders [ i ] ); //delete the shader's source - not needed after compilation
	}
}

void Shader::Bind ( ) const
{ 
	glUseProgram ( _program );
}

std::string Shader::LoadShader ( const std::string & fileName )
{
	std::ifstream file;
	file.open ( ( fileName ).c_str ( ) );
	std::string output;
	std::string line;
	if ( file.is_open ( ) )
	{
		while ( file.good ( ) )
		{
			getline ( file, line );
			output.append ( line + "\n" );
		}
	}
	else
	{
		std::cerr << "Unable to load shader: " << fileName << std::endl;
	}
	return output;
}

void Shader::CheckShaderError ( GLuint shader, GLuint flag, bool isProgram, const std::string & errorMessage )
{
	GLint success = 0;
	GLchar error [ 1024 ] = { 0 };
	if ( isProgram )
	{
		glGetProgramiv ( shader, flag, &success );
	}
	else
	{
		glGetShaderiv ( shader, flag, &success );
	}

	if ( success == GL_FALSE )
	{
		if ( isProgram )
		{
			glGetProgramInfoLog ( shader, sizeof ( error ), NULL, error );
		}
		else
		{
			glGetShaderInfoLog ( shader, sizeof ( error ), NULL, error );
		}

		std::cerr << errorMessage << ": '" << error << "'" << std::endl;
	}
}

GLuint Shader::CreateShader ( const std::string & text, GLenum type )
{
	auto shader = glCreateShader ( type );
	
	if ( shader == 0 ) //if == 0 shader no created
	{
		std::cerr << "Error type creation failed " << type << std::endl;
		return 0;
	}

	const GLchar * source [ 1 ] { nullptr };
	source[ 0 ] = text.c_str ( );
	glShaderSource ( shader, 1, source, NULL );
	glCompileShader ( shader );

	CheckShaderError ( shader, GL_COMPILE_STATUS, false, "Error compiling shader!" ); //check for compile error

	return shader;
}

#pragma region Uniform Sets

inline GLint Shader::GetUniformLocation ( const GLchar * name ) const
{
	auto location = glGetUniformLocation ( _program, name );
	return location;
}

void Shader::SetUniformByName ( const GLchar * name, const GLboolean v ) const
{
	glUniform1i ( GetUniformLocation ( name ), v );
}

void Shader::SetUniformByName ( const GLchar * name, const GLint v ) const
{
	glUniform1i ( GetUniformLocation ( name ), v );
}

void Shader::SetUniform ( const GLint location, const GLint v ) const
{ 
	glUniform1i ( location, v );
}

void Shader::SetUniform ( const GLint location, const float value ) const
{
	glUniform1f ( location, value );
}

void Shader::SetUniform ( const GLint location, const float x, const float y, const float z ) const
{
	glUniform3f ( location, x, y, z );
}

void Shader::SetUniform ( const GLint location , const float x , const float y , const float z , const float w ) const
{
	glUniform4f ( location , x , y , z , w );
}

void Shader::SetUniformByName ( const GLchar * name, const GLfloat v ) const
{ 
	glUniform1f ( GetUniformLocation ( name ), v );
}

void Shader::SetUniformByName ( const GLchar * name, const GLfloat x, const GLfloat y, const GLfloat z ) const
{ 
	glUniform3f ( GetUniformLocation ( name ), x, y, z );
}

void Shader::SetUniformByName ( const GLchar * name, const GLfloat x, const GLfloat y, const GLfloat z, const GLfloat w ) const
{
	glUniform4f ( GetUniformLocation ( name ), x, y, z, w );
}

void Shader::SetUniform ( const GLint location , const GLboolean v ) const
{
	glUniform1i ( location , v );
}

void Shader::SetUniform ( const GLint location , const glm::mat4 & matrix ) const
{
	glUniformMatrix4fv ( location , 1 , GLU_FALSE , glm::value_ptr ( matrix ) );
}

void Shader::SetUniformByName ( const GLchar * name, const glm::mat4 & matrix ) const
{
	glUniformMatrix4fv ( GetUniformLocation ( name ), 1, GLU_FALSE, glm::value_ptr ( matrix ) );
}

void Shader::SetDirectionalLight ( const DirectionalLight & light ) const
{
	auto textureUnit = light.GetShadowMap ( )->GetActiveBind ( );
	SetUniform ( _uniforms [ SHADOW_MAP_U ] , textureUnit );
	SetUniform ( _uniforms [ LIGHT_SPACE_MATRIX_U ] , light.GetLightSpaceMatrix ( ) );
	
	const glm::vec3 & lightPos = light.GetDirection ( );
	SetUniform ( _uniforms [ LIGHT_POSITION_U ] , lightPos.x, lightPos.y, lightPos.z );

	const glm::vec3 & lightColor = light.GetColor ( ) * light.GetIntensity ( );
	SetUniform ( _uniforms [ LIGHT_COLOUR_U ] , lightColor.x , lightColor.y , lightColor.z );
}

#pragma endregion


void Shader::Update ( Camera & camera, Transform & transform )
{ 
	glm::mat4 model = transform.GetModel ( ); 
	// The last parameter of glUnifromMatrix4fv the actual matrix data, 
	// but GLM stores their matrices' data in a way that doesn't always match OpenGL's expectations 
	// so we first convert the data with GLM's built-in function value_ptr.
	glUniformMatrix4fv ( _uniforms [ MODEL_U ], 1, GLU_FALSE, glm::value_ptr ( model ) );

	glm::mat4 view = camera.GetViewMatrix ( );
	glUniformMatrix4fv ( _uniforms [ VIEW_U ], 1, GLU_FALSE, glm::value_ptr ( view ) );

	glm::mat4 projection = camera.GetProjectionMatrix ( );
	glUniformMatrix4fv ( _uniforms [ PROJECTION_U ], 1, GLU_FALSE, glm::value_ptr ( projection ) );

	if ( _uniforms [ CAMERA_POS_U ] >= 0 )
	{
		auto & camPos = camera.GetGameObject ( ).GetTransform ( ).GetPosition ( );

		// send the camera position as a uniform (world space)
		glUniform3f ( _uniforms [ CAMERA_POS_U ], camPos.x , camPos.y , camPos.z );
	}
}

