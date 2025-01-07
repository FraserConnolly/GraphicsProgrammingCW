#include <string>
#include <iostream>

#include "FrameBuffer.h"
#include "Shader.h"
#include "Texture.h"
#include "Renderer.h"


FrameBuffer::FrameBuffer ( int w , int h, const std::string textureName ) :
	m_width ( w ) , 
	m_height ( h ),
	m_quadVAO ( 0 ) ,
	m_quadVBO ( 0 )
{
	glGenFramebuffers ( 1 , &m_FBO );
	glBindFramebuffer ( GL_FRAMEBUFFER , m_FBO );

	// create a colorbuffer for attachment texture
	m_texture = new Texture ( m_width , m_height );

	Renderer::RegisterTexture ( textureName , m_texture );

	// create a renderbuffer object for depth and stencil attachment 
	glGenRenderbuffers ( 1 , &m_RBO );
	glBindRenderbuffer ( GL_RENDERBUFFER , m_RBO );
	glRenderbufferStorage ( GL_RENDERBUFFER , GL_DEPTH24_STENCIL8 , m_width , m_height ); // use a single renderbuffer object for both a depth AND stencil buffer.
	glBindRenderbuffer ( GL_RENDERBUFFER , 0 );
	glFramebufferRenderbuffer ( GL_FRAMEBUFFER , GL_DEPTH_STENCIL_ATTACHMENT , GL_RENDERBUFFER , m_RBO ); // now actually attach it

	// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if ( glCheckFramebufferStatus ( GL_FRAMEBUFFER ) == GL_FRAMEBUFFER_COMPLETE ) 
	{
		std::cout << "FRAMEBUFFER:: Framebuffer is complete!" << std::endl;
		m_ready = true;
	}
	else 
	{
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		m_ready = false;
		glBindFramebuffer ( GL_FRAMEBUFFER , 0 );
		return;
	}

	// unbind framebuffer
	glBindFramebuffer ( GL_FRAMEBUFFER , 0 );

	PrepareQuad ( );
}

FrameBuffer::~FrameBuffer ( )
{
	Renderer::DeregisterTexture ( m_texture );
	delete m_texture;
}

void FrameBuffer::Bind ( )
{
	if ( !m_ready ) 
	{
		return;
	}

	glBindFramebuffer ( GL_FRAMEBUFFER , m_FBO );
	//glEnable ( GL_DEPTH_TEST );
	
	glClearColor ( 0.0f , 0.0f , 0.0f , 0.0f );
	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glEnable ( GL_BLEND );
	glBlendFunc ( GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA );
}

void FrameBuffer::Unbind ( )
{
	if ( !m_ready )
	{
		return;
	}

	glBindFramebuffer ( GL_FRAMEBUFFER , 0 );
}

void FrameBuffer::PrepareQuad ( )
{
	// vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.

	// FC 2024-11-08 I changed the quadVertices to be just the bottom left corner by changing the 
	// three position values that were 1.0 to 0.0.
	float quadVertices [ ] = {
		//positions   // texCoords
		-1.0f,  0.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 0.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  0.0f,  0.0f, 1.0f,
		 0.0f, -1.0f,  1.0f, 0.0f,
		 0.0f,  0.0f,  1.0f, 1.0f
	};

	// cube VAO
	glGenVertexArrays ( 1 , &m_quadVAO );
	glGenBuffers ( 1 , &m_quadVBO );
	glBindVertexArray ( m_quadVAO );
	glBindBuffer ( GL_ARRAY_BUFFER , m_quadVBO );
	glBufferData ( GL_ARRAY_BUFFER , sizeof ( quadVertices ) , &quadVertices , GL_STATIC_DRAW );

	glEnableVertexAttribArray ( 0 );
	glVertexAttribPointer ( 0 , 2 , GL_FLOAT , GL_FALSE , 4 * sizeof ( float ) , ( void * ) 0 );
	glEnableVertexAttribArray ( 1 );
	glVertexAttribPointer ( 1 , 2 , GL_FLOAT , GL_FALSE , 4 * sizeof ( float ) , ( void * ) ( 2 * sizeof ( float ) ) );
}

void FrameBuffer::RenderQuad ( Shader * fboShader )
{
	//glDisable ( GL_DEPTH_TEST );
	glClearColor ( 1.0f , 1.0f , 1.0f , 1.0f ); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
	glClear ( GL_COLOR_BUFFER_BIT );
	

	fboShader->Bind ( );
	//fboShader->SetUniform ( "invert" , true );
	//fboShader->SetUniform ( "grayScale" , true );
	glBindVertexArray ( m_quadVAO );

	GLint tu = Renderer::BindTexture ( m_texture );
	fboShader->SetUniform ( "screenTexture" , tu );
	glDrawArrays ( GL_TRIANGLES , 0 , 6 );
}
