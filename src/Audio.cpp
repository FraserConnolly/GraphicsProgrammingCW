#include "Audio.h"
#include "FMOD.h"
#include "Transform.h"

void Audio::Startup ( )
{
	s_pFmodIntegration = new FMOD_Intergration ( );
}

void Audio::Service ( )
{
	s_pFmodIntegration->Update ( );
}

void Audio::Shutdown ( )
{
	delete s_pFmodIntegration;
}

void Audio::LoadBank ( const std::string & strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags )
{
	auto tFoundIt = s_pFmodIntegration->m_Banks.find ( strBankName );

	if ( tFoundIt != s_pFmodIntegration->m_Banks.end ( ) )
	{
		return;
	}

	FMOD::Studio::Bank * pBank;
	FMOD_Intergration::ErrorCheck ( s_pFmodIntegration->mpStudioSystem->loadBankFile ( strBankName.c_str ( ), flags, &pBank ) );
	if ( pBank )
	{
		s_pFmodIntegration->m_Banks [ strBankName ] = pBank;
	}
}

void Audio::Set3dListenerAndOrientation ( const int listenerIndex, const Transform & transform )
{
	Set3dListenerAndOrientation ( listenerIndex, transform.GetPosition ( ), transform.GetForward ( ), transform.GetUp ( ) );
}

void Audio::Set3dListenerAndOrientation ( const int listenerIndex, const glm::vec3 & vPosition, const glm::vec3 & vForward, const glm::vec3 & vUp )
{
	FMOD_3D_ATTRIBUTES attributes { };

	attributes.position.x = vPosition.x;
	attributes.position.y = vPosition.y;
	attributes.position.z = vPosition.z;

	attributes.forward.x = vForward.x;
	attributes.forward.y = vForward.y;
	attributes.forward.z = vForward.z;
	
	attributes.up.x = vUp.x;
	attributes.up.y = vUp.y;
	attributes.up.z = vUp.z;

	FMOD_Intergration::ErrorCheck ( s_pFmodIntegration->mpStudioSystem->setListenerAttributes ( listenerIndex, &attributes ) );
}

#pragma region Sounds

void Audio::LoadSound ( const std::string & strSoundName, bool b3d, bool bLooping, bool bStream )
{
	auto tFoundIt = s_pFmodIntegration->m_Sounds.find ( strSoundName );
	if ( tFoundIt != s_pFmodIntegration->m_Sounds.end ( ) )
		return;

	FMOD_MODE eMode = FMOD_DEFAULT;
	eMode |= b3d ? FMOD_3D : FMOD_2D;
	eMode |= bLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
	eMode |= bStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

	FMOD::Sound * pSound = nullptr;
	FMOD_Intergration::ErrorCheck ( s_pFmodIntegration->mpSystem->createSound ( strSoundName.c_str ( ), eMode, nullptr, &pSound ) );
	if ( pSound )
	{
		s_pFmodIntegration->m_Sounds [ strSoundName ] = pSound;
	}
}

void Audio::UnLoadSound ( const std::string & strSoundName )
{
	auto tFoundIt = s_pFmodIntegration->m_Sounds.find ( strSoundName );
	if ( tFoundIt == s_pFmodIntegration->m_Sounds.end ( ) )
		return;

	FMOD_Intergration::ErrorCheck ( tFoundIt->second->release ( ) );
	s_pFmodIntegration->m_Sounds.erase ( tFoundIt );
}

int Audio::PlaySound ( const std::string & strSoundName, const glm::vec3 & vPosition, float fVolumedB )
{
	int nChannelId = s_pFmodIntegration->mnNextChannelId++;
	auto tFoundIt = s_pFmodIntegration->m_Sounds.find ( strSoundName );
	if ( tFoundIt == s_pFmodIntegration->m_Sounds.end ( ) )
	{
		LoadSound ( strSoundName );
		tFoundIt = s_pFmodIntegration->m_Sounds.find ( strSoundName );
		if ( tFoundIt == s_pFmodIntegration->m_Sounds.end ( ) )
		{
			return nChannelId;
		}
	}
	FMOD::Channel * pChannel = nullptr;
	FMOD_Intergration::ErrorCheck ( s_pFmodIntegration->mpSystem->playSound ( tFoundIt->second, nullptr, true, &pChannel ) );
	if ( pChannel )
	{
		FMOD_MODE currMode;
		tFoundIt->second->getMode ( &currMode );
		if ( currMode & FMOD_3D )
		{
			FMOD_VECTOR position = VectorToFmod ( vPosition );
			FMOD_Intergration::ErrorCheck ( pChannel->set3DAttributes ( &position, nullptr ) );
		}
		FMOD_Intergration::ErrorCheck ( pChannel->setVolume ( dbToVolume ( fVolumedB ) ) );
		FMOD_Intergration::ErrorCheck ( pChannel->setPaused ( false ) );
		s_pFmodIntegration->m_Channels [ nChannelId ] = pChannel;
	}
	return nChannelId;
}

void Audio::StopChannel ( int nChannelId )
{
	auto tFoundIt = s_pFmodIntegration->m_Channels.find ( nChannelId );
	if ( tFoundIt == s_pFmodIntegration->m_Channels.end ( ) )
		return;

	FMOD_Intergration::ErrorCheck ( tFoundIt->second->stop ( ) );
}

void Audio::StopAllChannels ( )
{
	for ( auto & channel : s_pFmodIntegration->m_Channels )
	{
		FMOD_Intergration::ErrorCheck ( channel.second->stop ( ) );
	}
}

void Audio::SetChannel3dPosition ( int nChannelId, const glm::vec3 & vPosition )
{
	auto tFoundIt = s_pFmodIntegration->m_Channels.find ( nChannelId );
	if ( tFoundIt == s_pFmodIntegration->m_Channels.end ( ) )
		return;

	FMOD_VECTOR position = VectorToFmod ( vPosition );
	FMOD_Intergration::ErrorCheck ( tFoundIt->second->set3DAttributes ( &position, NULL ) );
}

void Audio::SetChannelVolume ( int nChannelId, float fVolumedB )
{
	auto tFoundIt = s_pFmodIntegration->m_Channels.find ( nChannelId );
	if ( tFoundIt == s_pFmodIntegration->m_Channels.end ( ) )
		return;

	FMOD_Intergration::ErrorCheck ( tFoundIt->second->setVolume ( dbToVolume ( fVolumedB ) ) );
}

bool Audio::IsPlaying ( int nChannelId )
{
	auto tFoundIt = s_pFmodIntegration->m_Channels.find ( nChannelId );
	if ( tFoundIt == s_pFmodIntegration->m_Channels.end ( ) )
	{
		// channel not in use
		return false;
	}

	bool isPlaying = false;
	FMOD_Intergration::ErrorCheck ( tFoundIt->second->isPlaying ( &isPlaying ) );
	return isPlaying;
}

#pragma endregion

#pragma region FMOD Events

void Audio::LoadEvent ( const std::string & strEventName )
{
	auto tFoundit = s_pFmodIntegration->m_Events.find ( strEventName );
	if ( tFoundit != s_pFmodIntegration->m_Events.end ( ) )
		return;
	FMOD::Studio::EventDescription * pEventDescription = NULL;
	FMOD_Intergration::ErrorCheck ( s_pFmodIntegration->mpStudioSystem->getEvent ( strEventName.c_str ( ), &pEventDescription ) );
	if ( pEventDescription )
	{
		FMOD::Studio::EventInstance * pEventInstance = NULL;
		FMOD_Intergration::ErrorCheck ( pEventDescription->createInstance ( &pEventInstance ) );
		if ( pEventInstance )
		{
			s_pFmodIntegration->m_Events [ strEventName ] = pEventInstance;
		}
	}
}

void Audio::SetEvent3dAttributes ( const std::string & strEventName, const Transform & transform )
{
	auto tFoundit = s_pFmodIntegration->m_Events.find ( strEventName );
	if ( tFoundit == s_pFmodIntegration->m_Events.end ( ) )
	{
		return;
	}

	FMOD_3D_ATTRIBUTES attributes { };
	attributes.position.x = transform.GetPosition ( ).x;
	attributes.position.y = transform.GetPosition ( ).y;
	attributes.position.z = transform.GetPosition ( ).z;

	attributes.forward.x = transform.GetForward ( ).x;
	attributes.forward.y = transform.GetForward ( ).y;
	attributes.forward.z = transform.GetForward ( ).z;

	attributes.up.x = transform.GetUp ( ).x;
	attributes.up.y = transform.GetUp ( ).y;
	attributes.up.z = transform.GetUp ( ).z;

	FMOD_Intergration::ErrorCheck ( tFoundit->second->set3DAttributes ( &attributes ) );
}

void Audio::SetEvent3dAttributes ( const std::string & strEventName, const glm::vec3 & vPosition )
{
	auto tFoundit = s_pFmodIntegration->m_Events.find ( strEventName );
	if ( tFoundit == s_pFmodIntegration->m_Events.end ( ) )
	{
		return;
	}

	FMOD_3D_ATTRIBUTES attributes { };
	attributes.position.x = vPosition.x;
	attributes.position.y = vPosition.y;
	attributes.position.z = vPosition.z;

	// default right handed coordinates
	attributes.forward.z = -1;
	attributes.up.y = 1;

	FMOD_Intergration::ErrorCheck ( tFoundit->second->set3DAttributes ( &attributes ) );
}

void Audio::PlayEvent ( const std::string & strEventName )
{
	auto tFoundit = s_pFmodIntegration->m_Events.find ( strEventName );
	if ( tFoundit == s_pFmodIntegration->m_Events.end ( ) )
	{
		LoadEvent ( strEventName );
		tFoundit = s_pFmodIntegration->m_Events.find ( strEventName );
		if ( tFoundit == s_pFmodIntegration->m_Events.end ( ) )
			return;
	}
	tFoundit->second->start ( );
}

void Audio::StopEvent ( const std::string & strEventName, bool bImmediate )
{
	auto tFoundIt = s_pFmodIntegration->m_Events.find ( strEventName );
	if ( tFoundIt == s_pFmodIntegration->m_Events.end ( ) )
		return;

	FMOD_STUDIO_STOP_MODE eMode;
	eMode = bImmediate ? FMOD_STUDIO_STOP_IMMEDIATE : FMOD_STUDIO_STOP_ALLOWFADEOUT;
	FMOD_Intergration::ErrorCheck ( tFoundIt->second->stop ( eMode ) );
}

void Audio::GetEventParameter ( const std::string & strEventName, const std::string & strEventParameter, float & parameter )
{
	auto tFoundIt = s_pFmodIntegration->m_Events.find ( strEventName );
	if ( tFoundIt == s_pFmodIntegration->m_Events.end ( ) )
	{
		return;
	}

	float value;
	FMOD_Intergration::ErrorCheck ( tFoundIt->second->getParameterByName ( strEventParameter.c_str ( ), &value, &parameter ) );
}

void Audio::SetEventParameter ( const std::string & strEventName, const std::string & strParameterName, float fValue )
{
	auto tFoundIt = s_pFmodIntegration->m_Events.find ( strEventName );
	if ( tFoundIt == s_pFmodIntegration->m_Events.end ( ) )
		return;

	FMOD_Intergration::ErrorCheck ( tFoundIt->second->setParameterByName ( strParameterName.c_str ( ), fValue ) );
}

bool Audio::IsEventPlaying ( const std::string & strEventName )
{
	auto tFoundIt = s_pFmodIntegration->m_Events.find ( strEventName );
	if ( tFoundIt == s_pFmodIntegration->m_Events.end ( ) )
		return false;

	FMOD_STUDIO_PLAYBACK_STATE * state = NULL;
	if ( tFoundIt->second->getPlaybackState ( state ) == FMOD_STUDIO_PLAYBACK_PLAYING )
	{
		return true;
	}
	return false;
}

#pragma endregion

#pragma region Helpers

float  Audio::dbToVolume ( float dB )
{
	return powf ( 10.0f, 0.05f * dB );
}

float Audio::VolumeTodb ( float volume )
{
	return 20.0f * log10f ( volume );
}

FMOD_VECTOR Audio::VectorToFmod ( const glm::vec3 & vPosition )
{
	FMOD_VECTOR fVec { };
	fVec.x = vPosition.x;
	fVec.y = vPosition.y;
	fVec.z = vPosition.z;
	return fVec;
}

#pragma endregion

FMOD_Intergration * Audio::s_pFmodIntegration = nullptr;
