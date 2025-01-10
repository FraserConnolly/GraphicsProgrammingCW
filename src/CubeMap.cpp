#include "CubeMap.h"
#include "Renderer.h"
#include "Camera.h"
#include "Shader.h"
#include "Texture.h"

CubeMap::CubeMap ( ) :
	m_shader ( new Shader ( ) ), 
    m_texture ( new Texture ( ) ) ,
    m_deleteResources ( true )
{
	m_shader->LoadShaders ( "cubeMap.vert", "cubeMap.frag" );

    InitaliseSkybox ( );
}

CubeMap::CubeMap ( Shader * shader, Texture * texture ) :
    m_shader ( shader ) ,
    m_texture ( texture ) 
{
    InitaliseSkybox ( );
}

CubeMap::~CubeMap ( )
{ 
    if ( m_deleteResources )
    {
        delete m_shader;
        delete m_texture;
    }
    glDeleteVertexArrays ( 1, &m_skyboxVAO );
    glDeleteBuffers ( 1, &m_skyboxVBO );
}

void CubeMap::InitaliseSkybox ( )
{
    // skybox VAO
    glGenVertexArrays ( 1 , &m_skyboxVAO );
    glGenBuffers ( 1 , &m_skyboxVBO );
    glBindVertexArray ( m_skyboxVAO );
    glBindBuffer ( GL_ARRAY_BUFFER , m_skyboxVBO );
    glBufferData ( GL_ARRAY_BUFFER , sizeof ( m_skyboxVertices ) , &m_skyboxVertices , GL_STATIC_DRAW );
    glEnableVertexAttribArray ( 0 );
    glVertexAttribPointer ( 0 , 3 , GL_FLOAT , GL_FALSE , 3 * sizeof ( float ) , ( void * ) 0 );
}

void CubeMap::LoadCubeMap ( const std::vector<char *> & cubeMapFilePaths )
{ 
	m_texture->LoadCubeMap ( cubeMapFilePaths );
}

void CubeMap::Draw ( Camera & camera )
{ 
   // draw skybox as last
    glDepthFunc ( GL_LEQUAL );  // change depth function so depth test passes when values are equal to depth buffer's content
    
    m_shader->Bind ( );

    m_shader->SetUniformByName ( "view", camera.GetViewMatrixNoTranslation( ) );
    m_shader->SetUniformByName ( "projection", camera.GetProjectionMatrix ( ) );
    
    // skybox cube
    glBindVertexArray ( m_skyboxVAO );
    auto textureUnit = Renderer::BindTexture ( m_texture );
    m_shader->SetUniformByName ( "skybox", textureUnit );

    glDrawArrays ( GL_TRIANGLES, 0, 36 );
    glBindVertexArray ( 0 );
    glDepthFunc ( GL_LESS ); // set depth function back to default
}
