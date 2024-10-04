#include "KeyboardInput.h"
#include "Time.h"

KeyboardInput::KeyboardInput ( )  
{
    StartUp ( );
}

KeyboardInput::~KeyboardInput ( )
{
    if (m_ready)
    {
    }

    for ( auto & key : m_keyRegistrations )
    {
        delete key.second;
    }

    m_keyRegistrations.clear ( );
}

void KeyboardInput::Service ( )
{
    if ( !m_ready )
    {
        return;
    }

    for ( auto & key : m_keyRegistrations )
    {
        // key is down
        if ( key.second->isPressed )
        {
            key.second->pressedDuration += Time::GetDeltaTime( );
        }
    }
}

bool KeyboardInput::registerKey ( int keyCode )
{
    if ( !m_ready )
    {
        return false;
    }

    if ( m_keyRegistrations.find ( keyCode ) != m_keyRegistrations.end ( ) )
    {
        // this key has already been registered.
        return true;
    }

    auto data = new keyEventStatus ( );
    data->onPressCallback = nullptr;

    m_keyRegistrations [ keyCode ] = data;
    return true;
}

bool KeyboardInput::registerOnKeyPressed ( const int keyCode, std::function<void ( int )> callback )
{
    if ( !m_ready )
    {
        return false;
    }

    if ( m_keyRegistrations.find ( keyCode ) != m_keyRegistrations.end ( ) )
    {
        // this key has already been registered.
        delete m_keyRegistrations [ keyCode ];
    }

    auto data = new keyEventStatus ( );
    data->onPressCallback = callback;

    m_keyRegistrations [ keyCode ] = data;
    return true;
}

bool KeyboardInput::wasPressedThisFrame ( const int keyCode )
{
    auto const status = m_keyRegistrations [ keyCode ];
    return status->isPressed && status->pressedDuration == 0;
}

bool KeyboardInput::isPressed ( const int keyCode )
{
    return m_keyRegistrations [ keyCode ]->isPressed;
}

void KeyboardInput::resetKeyPresses ( )
{ 
    for ( auto & key : m_keyRegistrations )
    {
        key.second->isPressed = false;
        key.second->pressedDuration = 0;
    }
}

void KeyboardInput::processKeyEvent ( const int keyCode, const bool pressed )
{ 
    if ( m_keyRegistrations.find ( keyCode ) == m_keyRegistrations.end ( ) )
	{
        return; // key not registered
	}

    const pKeyEventStatus key = m_keyRegistrations [ keyCode ];

    if ( pressed ) 
    {
        key->isPressed = true;

        // if this is the first frame for this scene then don't process on pressed or wasPressedThisFrame.
        key->pressedDuration = Time::GetDeltaTime( ) > 0 ? 0 : 0.001f;
        if ( key->onPressCallback != nullptr && Time::GetDeltaTime ( ) > 0 )
        {
            key->onPressCallback ( keyCode );
        }
    }
    else
    {
        // key is up
        key->isPressed = false;
    }
}

void KeyboardInput::StartUp ( )
{
    m_ready = true;
}
