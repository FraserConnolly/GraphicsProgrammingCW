#include "CubeMap.h"
#include "Renderer.h"
#include "Camera.h"

CubeMap::CubeMap ( ) :
	m_shader ( *new Shader ( ) ), 
    m_texture ( *new Texture ( ) ) ,
    m_camera ( nullptr )
{
	m_shader.LoadShaders ( "cubeMap.vert", "cubeMap.frag" );

    float skyboxVertices[ ] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    // skybox VAO
    glGenVertexArrays ( 1, &m_skyboxVAO );
    glGenBuffers ( 1, &m_skyboxVBO );
    glBindVertexArray ( m_skyboxVAO );
    glBindBuffer ( GL_ARRAY_BUFFER, m_skyboxVBO );
    glBufferData ( GL_ARRAY_BUFFER, sizeof ( skyboxVertices ), &skyboxVertices, GL_STATIC_DRAW );
    glEnableVertexAttribArray ( 0 );
    glVertexAttribPointer ( 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof ( float ), ( void * ) 0 );

}

CubeMap::~CubeMap ( )
{ 
	delete &m_shader;
	delete &m_texture;
    glDeleteVertexArrays ( 1, &m_skyboxVAO );
    glDeleteBuffers ( 1, &m_skyboxVBO );
}

void CubeMap::LoadCubeMap ( const std::vector<char *> & cubeMapFilePaths )
{ 
	m_texture.LoadCubeMap ( cubeMapFilePaths );
}

void CubeMap::SetCamera ( Camera & camera )
{ 
	m_shader.SetCamera ( &camera );
    m_camera = &camera;
}

void CubeMap::Draw ( )
{ 
    if ( m_camera == nullptr )
    {
        return;
    }

   // draw skybox as last
    glDepthFunc ( GL_LEQUAL );  // change depth function so depth test passes when values are equal to depth buffer's content
    
    m_shader.Bind ( );
    glm::mat4 view = glm::mat4 ( glm::mat3 ( m_camera->GetViewMatrix ( ) ) ); // remove translation from the view matrix
    m_shader.SetUniform ( "view", view );
    m_shader.SetUniform ( "projection", m_camera->GetProjectionMatrix ( ) );
    
    // skybox cube
    glBindVertexArray ( m_skyboxVAO );
    auto textureUnit = Renderer::BindTexture ( &m_texture );
    m_shader.SetUniform ( "skybox", textureUnit );

    glDrawArrays ( GL_TRIANGLES, 0, 36 );
    glBindVertexArray ( 0 );
    glDepthFunc ( GL_LESS ); // set depth function back to default
}
