#include "Display.h"


Display::Display()
	: _glContext ( 0 )
{
	_window = nullptr; //initialise to generate null access violation for debugging. 
	_screenWidth = 1920 * 0.95;
	_screenHeight = 1080 * 0.95; 
}

Display::~Display()
{
	SDL_DestroyWindow ( _window );
	SDL_GL_DeleteContext ( _glContext );
	SDL_Quit ( );
}

void Display::returnError(std::string errorString)
{
	cout << errorString << '\n';
	cout << "Press any key to quit.";
	char c;
	cin >> c;

	SDL_Quit();
}

void Display::swapBuffer()
{
	SDL_GL_SwapWindow(_window);
}

/// <summary>
/// Time in seconds since the game started
/// </summary>
float Display::getTime ( )
{
	return SDL_GetTicks ( ) / 1000.0f;
}

int Display::getWidth ( ) const
{
	return _screenWidth;
}

int Display::getHeight ( ) const
{
	return _screenHeight;
}

void Display::initDisplay()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	_window = SDL_CreateWindow("Offstage Controls",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		_screenWidth,
		_screenHeight, SDL_WINDOW_OPENGL);

	if (_window == nullptr)
	{
		returnError("Failed to create window.");
		return;
	}

	_glContext = SDL_GL_CreateContext(_window);

	if (_glContext == nullptr)
	{
		returnError("Failed to OpenGL context.");
		return;
	}

	GLenum error = glewInit();

	if (error != GLEW_OK)
	{
		returnError("GLEW failed to initialise.");
		return;
	}

	// enable depth testing
	glEnable ( GL_DEPTH_TEST );

	// enable back face culling
	glEnable ( GL_CULL_FACE );

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

}

void Display::initDisplay ( const float width, const float height )
{ 
	_screenWidth = (int) width;
	_screenHeight = (int) height;
	initDisplay ( );
}

void Display::clearDisplay ( )
{ 
	clearDisplay ( 0.0f, 0.0f, 0.0f, 1.0f );
}

void Display::clearDisplay ( const float r, const float g, const float b, const float a )
{ 
	glClearColor ( r, g, b, a );
	// glClearDepth ( 1.0 ); // removed during lab 4
	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); // clear colour and depth buffer - set colour to colour defined in glClearColor
}
