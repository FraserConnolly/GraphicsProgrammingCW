#pragma once
#include <GL/glew.h>

class Shader;
class Texture;

class FrameBuffer
{

public:
	FrameBuffer( int w , int h, const std::string textureName );
	~FrameBuffer();

	void Bind ( );
	void Unbind ( );
	void RenderQuad ( Shader * fboShader );

	Texture * GetTexture ( ) const { return m_texture; }

private:

	void PrepareQuad ( );

	bool m_ready = false;

	int m_width;
	int m_height;

	Texture * m_texture = nullptr;

	GLuint m_FBO = 0; // Frame Buffer 
	GLuint m_RBO = 0; // Render Buffer 
	GLuint m_CBO = 0; // Colour Buffer 
	GLuint m_quadVAO = 0;
	GLuint m_quadVBO = 0;

};

