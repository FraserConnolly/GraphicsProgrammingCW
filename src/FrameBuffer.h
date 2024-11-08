#pragma once
#include <GL/glew.h>
class FrameBuffer
{

public:
	FrameBuffer( int w , int h );
	~FrameBuffer();

	void Bind ( );
	void Unbind ( );

private:

	void PrepareQuad ( );
	void RenderQuad ( );

	bool m_ready = false;

	int m_width;
	int m_height;

	GLuint m_FBO; // Frame Buffer 
	GLuint m_RBO; // Render Buffer 
	GLuint m_CBO; // Colour Buffer 
	GLuint m_quadVAO;
	GLuint m_quadVBO;

};

