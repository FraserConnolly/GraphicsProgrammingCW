#include "Time.h"

void Time::StartUp ( )
{
	s_instance = new Time ( );
}

void Time::Service ( float newTime )
{ 

	s_instance->m_deltaTime = newTime - s_instance->m_time;
	s_instance->m_time = newTime;

	// If delta time is too large (more than 1 second), we must have resumed from a breakpoint.
	// Frame-lock to the target rate of 30fps.
	// From Game Engine Architecture 3rd Edition by Jason Gregory 8.5.5 
	if ( s_instance->m_deltaTime > 1.0f )
	{
		s_instance->m_deltaTime = 1.0f / 30.0f;
	}

	s_instance->m_frameCount++;
}

void Time::Shutdown ( )
{ 
	delete s_instance;
}

float Time::GetTime ( )
{
	return s_instance->m_time;
}

float Time::GetDeltaTime ( )
{
	return s_instance->m_deltaTime;
}

unsigned int Time::GetFrameCount ( )
{
	return s_instance->m_frameCount;
}

Time::Time ( ) : 
	m_time ( 0.0f ), 
	m_deltaTime ( 0.0f ), 
	m_frameCount( 0 )
{ 

}

Time::~Time ( )
{ }

Time * Time::s_instance = nullptr;