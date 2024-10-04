#pragma once
#include "KeyboardInput.h"
#include "MouseInput.h"

/// <summary>
/// A static class that will handle all input from the user
/// </summary>

class Input
{ 
	friend class GameEngine;

public:

#pragma region Keyboard functions


	// Keyboard functions

	static bool RegisterKey ( int keyCode );
	static bool RegisterOnKeyPressed ( int keyCode, std::function<void ( int )> callback );

	static bool WasKeyPressedThisFrame ( int keyCode );
	static bool IsKeyPressed ( int keyCode );

	// Call this whenever the scene loads to ensure that key holds are reset
	static void ResetKeyPresses ( );

#pragma endregion

#pragma region Mouse Functions

	static const glm::ivec2 & Input::GetMousePosition ( );
	static const glm::ivec2 & Input::GetMouseDelta ( );
	static const glm::ivec2 & Input::GetMouseWheelDelta ( );
	static const bool Input::GetButtonStateLeft ( );
	static const bool Input::GetButtonStateRight ( );


#pragma endregion


private:

	static void StartUp ( );
	static void Shutdown ( );
	static void Service ( );

	static Input * s_instance;

	KeyboardInput * m_keyboardInput;
	MouseInput * m_mouseInput;

	static void ProcessKeyEvent ( const int keyCode, const bool pressed );
	static void ProcessWheel ( const int horizontalMovement, const int verticalMovement );

};

