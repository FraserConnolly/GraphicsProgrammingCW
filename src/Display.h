#pragma once
#include <SDL/SDL.h>
#include <GL\glew.h>
#include <iostream>
#include <string>

using namespace std;

class Display
{

public:
	
	Display();
	~Display();
	
	void initDisplay ( );
	void initDisplay ( const int width, const int height );
	void initDisplay ( const int width , const int height , const string title );
	
	void clearDisplay ( );
	void clearDisplay ( const float r, const float g, const float b, const float a);
	
	void swapBuffer();
	
	float getTime ( );

	int getWidth  ( ) const;
	int getHeight ( ) const;

private:

	void returnError(std::string errorString);

	SDL_Window* _window; //holds pointer to out window
	SDL_GLContext _glContext;
	int _screenWidth;
	int _screenHeight;
	string _title = "Offstage Controls";

};

