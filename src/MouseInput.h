#pragma once
#include <glm/glm.hpp> 
#include <SDL/SDL.h>

class MouseInput
{

	friend class Input;

public:

	void ProcessWheel ( const int horizontalMovement, const int verticalMovement )
	{
		_wheelOffset.x = horizontalMovement;
		_wheelOffset.y = verticalMovement;
	}

#pragma region Getters
	
	const glm::ivec2 & GetMouseOffset ( ) const
	{
		return _mouseOffset;
	}

	const int GetXOffset ( ) const
	{
		return _mouseOffset.y;
	}
	
	const int GetYOffset ( ) const
	{
		return _mouseOffset.y;
	}

	const glm::ivec2 & GetMousePosition ( ) const
	{
		return _mousePosition;
	}

	const int GetXPosition ( ) const
	{
		return _mousePosition.y;
	}

	const int GetYPosition ( ) const
	{
		return _mousePosition.y;
	}

	const glm::ivec2 & GetMouseWheelOffset ( ) const
	{
		return _wheelOffset;
	}

	const int GetHorizontalWheelOffset ( ) const
	{
		return _wheelOffset.x;
	}

	const int GetVerticalWheelOffset ( ) const
	{
		return _wheelOffset.y;
	}

	const bool GetIsLeftClicked ( ) const
	{
		return _buttonState & SDL_BUTTON ( SDL_BUTTON_LEFT );
	}

	const bool GetIsRightClicked ( ) const
	{
		return _buttonState & SDL_BUTTON ( SDL_BUTTON_RIGHT );
	}

#pragma endregion

private:

	glm::ivec2 _mouseOffset;
	glm::ivec2 _mousePosition;
	glm::ivec2 _wheelOffset;
	unsigned int _buttonState = 0u;

	void Service ( ) 
	{
		_buttonState = SDL_GetRelativeMouseState ( &_mouseOffset.x, &_mouseOffset.y );
		SDL_GetMouseState ( &_mousePosition.x, &_mousePosition.y );

		// reset wheel offset each frame
		_wheelOffset.x = 0;
		_wheelOffset.y = 0;
	}

	MouseInput ( ) { }
	~MouseInput ( )	{ }
};

