#pragma once

#include <GL\glew.h>
#include <vector>

class Camera;
class Texture;
class Shader;

class CubeMap
{

public:

	CubeMap ( );
	~CubeMap ( );

	void LoadCubeMap ( const std::vector<char *> & cubeMapFilePaths );
	void SetCamera ( Camera & camera );
	void Draw ( );

private:

	Shader & m_shader;
	Texture & m_texture;
	Camera * m_camera;
	GLuint m_skyboxVAO;
	GLuint m_skyboxVBO;

};

