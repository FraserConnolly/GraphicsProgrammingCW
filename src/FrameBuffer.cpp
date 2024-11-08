#include "FrameBuffer.h"
#include <iostream>


FrameBuffer::FrameBuffer ( int w , int h ) : 
	m_width ( w ) , 
	m_height ( h )
{
	glGenFramebuffers ( 1 , &m_FBO );
	glBindFramebuffer ( GL_FRAMEBUFFER , m_FBO );

	// create a colorbuffer for attachment texture
	glGenTextures ( 1 , &m_CBO );
	glBindTexture ( GL_TEXTURE_2D , m_CBO );
	glTexImage2D ( GL_TEXTURE_2D , 0 , GL_RGB , m_width , m_height , 0 , GL_RGB , GL_UNSIGNED_BYTE , NULL );
	glTexParameteri ( GL_TEXTURE_2D , GL_TEXTURE_MIN_FILTER , GL_LINEAR );
	glTexParameteri ( GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER , GL_LINEAR );
	glFramebufferTexture2D ( GL_FRAMEBUFFER , GL_COLOR_ATTACHMENT0 , GL_TEXTURE_2D , m_CBO , 0 );

	// create a renderbuffer object for depth and stencil attachment 
	glGenRenderbuffers ( 1 , &m_RBO );
	glBindRenderbuffer ( GL_RENDERBUFFER , m_RBO );
	glRenderbufferStorage ( GL_RENDERBUFFER , GL_DEPTH24_STENCIL8 , w , h ); // use a single renderbuffer object for both a depth AND stencil buffer.
	glBindRenderbuffer ( GL_RENDERBUFFER , 0 );
	glFramebufferRenderbuffer ( GL_FRAMEBUFFER , GL_DEPTH_STENCIL_ATTACHMENT , GL_RENDERBUFFER , m_RBO ); // now actually attach it

	// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if ( glCheckFramebufferStatus ( GL_FRAMEBUFFER ) == GL_FRAMEBUFFER_COMPLETE ) {
		std::cout << "FRAMEBUFFER:: Framebuffer is complete!" << std::endl;
		m_ready = true;
	}
	glBindFramebuffer ( GL_FRAMEBUFFER , 0 );
}

FrameBuffer::~FrameBuffer ( )
{
}

void FrameBuffer::Bind ( )
{
	glBindFramebuffer ( GL_FRAMEBUFFER , m_FBO );
	glEnable ( GL_DEPTH_TEST );
	glClearColor ( 0.1f , 0.1f , 0.1f , 1.0f );
	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void FrameBuffer::Unbind ( )
{
	glBindFramebuffer ( GL_FRAMEBUFFER , 0 );
}

void FrameBuffer::PrepareQuad ( )
{
	// vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.

	float quadVertices [ ] = {
		//positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
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

void FrameBuffer::RenderQuad ( )
{
	glDisable ( GL_DEPTH_TEST );
	glClearColor ( 1.0f , 1.0f , 1.0f , 1.0f ); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
	glClear ( GL_COLOR_BUFFER_BIT );

	FBOShader.Bind ( );
	glBindVertexArray ( m_quadVAO );
	glBindTexture ( GL_TEXTURE_2D , m_CBO );	// use the color attachment texture as the texture of the quad plane
	glDrawArrays ( GL_TRIANGLES , 0 , 6 );
}
