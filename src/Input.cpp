#include "Input.h"

#pragma region Mechanics

void Input::StartUp ( )
{ 
	s_instance = new Input ( );
	s_instance->m_keyboardInput = new KeyboardInput ( );
	s_instance->m_mouseInput = new MouseInput ( );
}

void Input::Shutdown ( )
{ 
	delete s_instance->m_keyboardInput;
	delete s_instance->m_mouseInput;
	delete s_instance;
}

void Input::Service ( )
{ 
	s_instance->m_keyboardInput->Service ( );
	s_instance->m_mouseInput->Service ( );
}

#pragma endregion

#pragma region Keyboard

void Input::ProcessKeyEvent ( const int keyCode, const bool pressed )
{ 
	s_instance->m_keyboardInput->processKeyEvent ( keyCode, pressed );
}

void Input::ProcessWheel ( const int horizontalMovement, const int verticalMovement )
{ 
	s_instance->m_mouseInput->ProcessWheel ( horizontalMovement, verticalMovement );
}

bool Input::RegisterKey ( int keyCode )
{
	return s_instance->m_keyboardInput->registerKey ( keyCode );
}

bool Input::RegisterOnKeyPressed ( int keyCode, std::function<void ( int )> callback )
{
	return s_instance->m_keyboardInput->registerOnKeyPressed ( keyCode, callback );
}

bool Input::WasKeyPressedThisFrame ( int keyCode )
{
	return s_instance->m_keyboardInput->wasPressedThisFrame ( keyCode );
}

bool Input::IsKeyPressed ( int keyCode )
{
	return s_instance->m_keyboardInput->isPressed ( keyCode );
}

void Input::ResetKeyPresses ( )
{ 
	s_instance->m_keyboardInput->resetKeyPresses ( );
}

#pragma endregion

#pragma region Mouse

const glm::ivec2 & Input::GetMousePosition ( )
{
	return s_instance->m_mouseInput->GetMousePosition ( );
}

const glm::ivec2 & Input::GetMouseDelta ( )
{
	return s_instance->m_mouseInput->GetMouseOffset ( );
}

const glm::ivec2 & Input::GetMouseWheelDelta ( )
{
	return s_instance->m_mouseInput->GetMouseWheelOffset ( );
}

const bool Input::GetButtonStateLeft ( )
{
	return s_instance->m_mouseInput->GetIsLeftClicked ( );
}

const bool Input::GetButtonStateRight ( )
{
	return s_instance->m_mouseInput->GetIsRightClicked ( );
}

#pragma endregion

Input * Input::s_instance = nullptr;