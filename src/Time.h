#pragma once

/// <summary>
/// Static class that provides time related functions
/// </summary>

class Time
{

	friend class GameEngine;

public:
	
	static void StartUp ( );
	static void Service ( float newTime );
	static void Shutdown ( );

	// Returns the time in seconds
	static float GetTime( );

	// Returns the delta time in seconds
	static float GetDeltaTime( );

	static unsigned int GetFrameCount ( );

private:

	static Time * s_instance;

	// in seconds
	float m_time = 0.0f;
	float m_deltaTime = 0.0f;
	unsigned int m_frameCount = 0;

	Time ( );
	~Time ( );

};

