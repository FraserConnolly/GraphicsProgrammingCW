#include "Audio Event Emitter.h"


/// <summary>
/// Loads the event into the system if it isn't already.
/// This will also create the FMOD event instance.
/// </summary>
/// <param name="strEventName">These are FMOD events so don't forget to use the full FMOD path which will start event:/</param>

AudioEventEmitter::AudioEventEmitter ( GameObject & hostObject ) :
	Component ( hostObject, ComponentTypes::AUDIO_EVENT_EMITTER ),
	m_playOnCollisionEnter ( false ), m_stopOnCollisionExit ( false )
{ }

void AudioEventEmitter::LoadEvent ( const std::string & strEventName )
{
	m_eventName = strEventName; // copy string
	Audio::LoadEvent ( m_eventName );
	m_position = m_transform.GetPosition ( );
	Audio::SetEvent3dAttributes ( m_eventName, m_transform );
}

inline void AudioEventEmitter::PlayEvent ( )
{
	if ( m_eventName.empty ( ) )
	{
		return;
	}

	Audio::PlayEvent ( m_eventName );
}

inline void AudioEventEmitter::StopEvent ( )
{
	if ( m_eventName.empty ( ) )
	{
		return;
	}

	Audio::StopEvent ( m_eventName );
}

inline bool AudioEventEmitter::IsPlaying ( )
{
	if ( m_eventName.empty ( ) )
	{
		return false;
	}

	return Audio::IsEventPlaying ( m_eventName );
}

inline void AudioEventEmitter::Update ( )
{
	if ( !IsPlaying ( ) )
	{
		return;
	}

	auto & newPosition = m_transform.GetPosition ( );
	if ( newPosition != m_position )
	{
		m_position = newPosition;
		Audio::SetEvent3dAttributes ( m_eventName, m_transform );
	}
}

void AudioEventEmitter::OnCollisionEnter ( const Collider & other )
{ 
	if ( !m_playOnCollisionEnter )
	{
		return;
	}

	PlayEvent ( );
}

void AudioEventEmitter::OnCollisionExit ( const Collider & other )
{ 
	if ( !m_stopOnCollisionExit )
	{
		return;
	}

	StopEvent ( );
}

void AudioEventEmitter::SetCollisionBehaviour ( bool startOnCollisionEnter, bool stopOnCollisionExit )
{ 
	m_playOnCollisionEnter = startOnCollisionEnter;
	m_stopOnCollisionExit = stopOnCollisionExit;
}

void AudioEventEmitter::Deserialise ( const json & data )
{
	bool startOnEnter = false;
	bool stopOnExit = false;

	if ( data.contains ( "EventName" ) )
	{
		auto eventName = data [ "EventName" ].get <std::string> ( );
		LoadEvent ( eventName );
	}

	if ( data.contains ( "StartOnEnter" ) )
	{
		startOnEnter = data [ "StartOnEnter" ].get<bool> ( );
	}

	if ( data.contains ( "StopOnExit" ) )
	{
		stopOnExit = data [ "StopOnExit" ].get<bool> ( );
	}

	SetCollisionBehaviour ( startOnEnter , stopOnExit );

}
