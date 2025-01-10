#pragma once
#include <GL/glew.h>
#include <string>

class Shader;
class Texture;

class FrameBuffer
{

public:
	FrameBuffer( int w , int h, const std::string textureName, bool depthOnly = false );
	~FrameBuffer();

	void Bind ( );
	void Unbind ( );
	void RenderQuad ( Shader * fboShader );

	Texture * GetTexture ( ) const { return m_texture; }

	int GetWidth ( ) const { return m_width; }
	int GetHeight ( ) const { return m_height; }

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

