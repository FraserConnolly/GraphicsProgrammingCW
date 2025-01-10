#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class FrameBuffer;
class Shader;
class Texture;

class DirectionalLight
{

public:
	
	DirectionalLight ( FrameBuffer * depthBuffer, Shader * depthShader );
	~DirectionalLight ();

	void Bind ( ) const;
	void Unbind ( ) const;

	void SetDirection ( const glm::vec3 & direction ) { m_direction = direction; };
	const glm::vec3 & GetDirection ( ) const { return m_direction; };

	const glm::mat4 GetLightSpaceProjection ( ) const { return m_lightSpaceProjection; };
	const glm::mat4 GetLightViewTransformMatrix ( ) const
	{
		return glm::lookAt ( 
			m_direction ,
			glm::vec3 ( 0.0f , 0.0f , 0.0f ) ,
			glm::vec3 ( 0.0f , 1.0f , 0.0f ) );
	};
	const glm::mat4 GetLightSpaceMatrix ( ) const
	{
		return m_lightSpaceProjection * GetLightViewTransformMatrix ( );
	};

	void SetModelInShader ( const glm::mat4 & model ) const;
	Shader * GetDepthShader ( ) const { return m_depthShader; };
	Texture * GetShadowMap ( ) const;

private:
	
	FrameBuffer * m_directionalShadowMap;
	Shader * m_depthShader;
	
	glm::vec3 m_direction = glm::vec3();
	glm::mat4 m_lightSpaceProjection;

	float m_nearPlane	= 0.5f;
	float m_farPlane	= 20.0f;
	float m_orthoSize	= 10.0f;

	GLint m_lightSpaceMatrixLocation = -1;
	GLint m_modelLocation = -1;
};

