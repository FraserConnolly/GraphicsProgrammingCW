#pragma once

#include "Component.h"
#include "GameObject.h"
#include "Audio.h"

class Collider;

class AudioEventEmitter : public Component
{ 
	friend class GameObject;

private:

	AudioEventEmitter ( GameObject & hostObject );
	~AudioEventEmitter () { }
	
public:

	/// <summary>
	/// Loads the event into the system if it isn't already.
	/// This will also create the FMOD event instance.
	/// </summary>
	/// <param name="strEventName">These are FMOD events so don't forget to use the full FMOD path which will start event:/</param>
	void LoadEvent ( const std::string & strEventName );

	void PlayEvent ( );
	void StopEvent ( );
	bool IsPlaying ( );
	
	void Update ( ) override;
	void OnCollisionEnter ( const Collider & other ) override;
	void OnCollisionExit  ( const Collider & other ) override;

	void SetCollisionBehaviour ( bool startOnCollisionEnter, bool stopOnCollisionExit );

private:

	glm::vec3 m_position;
	std::string m_eventName;
	bool m_playOnCollisionEnter;
	bool m_stopOnCollisionExit;

};

