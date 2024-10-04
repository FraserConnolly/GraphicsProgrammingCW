#pragma once

#include "GameObject.h"
#include "Audio.h"

class AudioListener : public Component
{ 
	friend class GameObject;

private:

	AudioListener ( GameObject & hostObject ) : 
		Component( hostObject, ComponentTypes::AUDIO_LISTENER )
	{ }

	~AudioListener () { }
	
public:

	void Update ( );
	
private:

	// for future use
	int m_listenerIndex = 0;

	glm::vec3 m_position;
	glm::vec3 m_forward;
	glm::vec3 m_up;

};

