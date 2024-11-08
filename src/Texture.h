#pragma once 

#include <string> 
#include <vector>
#include <GL\glew.h>

class Texture
{
	friend class Renderer;
	enum TextureType;

public: 
	Texture ( );
	Texture ( const std::string & fileName );
	Texture ( const char * fileName );
	Texture ( const int width , const int height ); // used for render textures.

	~Texture ( );
	
	void LoadTexture ( const char * filename );
	void LoadCubeMap ( const std::vector<char *> & cubeMapFilePaths );

	// bind up to 80 in OpenGL 4 textures. 
	// true limit is determined at runtime.
	// On Nvidia GForce 1050TI it is 192.
	void Bind ( GLint unit ); 

protected: 

private:

	void SetDefaultTexture ( );
	void DeleteCurrentTexture ( );

	GLuint _texture = 0u;
	int _width = 0;
	int	_height = 0;
	int _nrChannels = 0;
	bool _fileLoaded = false;
	GLint _activeBind = -1;
	TextureType m_textureType;

};