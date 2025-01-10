#include "DirectionalLight.h"

#include "FrameBuffer.h"
#include "Shader.h"

DirectionalLight::DirectionalLight ( FrameBuffer * depthBuffer, Shader * depthShader ) : 
	m_directionalShadowMap ( depthBuffer ), 
	m_depthShader ( depthShader )
{
	m_lightSpaceProjection = glm::ortho ( -m_orthoSize , m_orthoSize , -m_orthoSize , m_orthoSize , m_nearPlane , m_farPlane );
	m_lightSpaceMatrixLocation = m_depthShader->GetUniformLocation ( "lightSpaceMatrix" );
	m_modelLocation = m_depthShader->GetUniformLocation ( "model" );
}

DirectionalLight::~DirectionalLight ( ) { }

void DirectionalLight::Bind ( ) const
{
	m_depthShader->Bind ( );
	m_depthShader->SetUniform ( m_lightSpaceMatrixLocation , GetLightSpaceMatrix ( ) );
	m_directionalShadowMap->Bind ( );
}

void DirectionalLight::Unbind ( ) const
{
	m_directionalShadowMap->Unbind ( );
}

void DirectionalLight::SetModelInShader ( const glm::mat4 & model ) const
{
	m_depthShader->SetUniform ( m_modelLocation , model );
}

Texture * DirectionalLight::GetShadowMap ( ) const
{ 
	return m_directionalShadowMap->GetTexture ( ); 
}
