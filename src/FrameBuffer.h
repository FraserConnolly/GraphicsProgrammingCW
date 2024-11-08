#pragma once
#include <GL/glew.h>

class Shader;
class Texture;

class FrameBuffer
{

public:
	FrameBuffer( int w , int h );
	~FrameBuffer();

	void Bind ( );
	void Unbind ( );
	void RenderQuad ( Shader * fboShader );

private:

	void PrepareQuad ( );

	bool m_ready = false;

	int m_width;
	int m_height;

	Texture * m_texture = nullptr;

	GLuint m_FBO; // Frame Buffer 
	GLuint m_RBO; // Render Buffer 
	GLuint m_CBO; // Colour Buffer 
	GLuint m_quadVAO;
	GLuint m_quadVBO;

};

