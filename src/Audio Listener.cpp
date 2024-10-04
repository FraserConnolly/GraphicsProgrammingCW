#include "Audio Listener.h"

void AudioListener::Update ( )
{
	auto & newPosition = m_transform.GetPosition ( );
	auto & newForward = m_transform.GetUp ( );
	auto & newUp = m_transform.GetForward ( );
	if ( m_position != newPosition || m_forward != newForward || m_up != newUp )
	{
		m_position = newPosition;
		m_forward = newForward;
		m_up = newUp;

		Audio::Set3dListenerAndOrientation ( m_listenerIndex, m_position, m_forward, m_up );
	}
}
