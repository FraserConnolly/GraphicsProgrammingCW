#include "FMOD.h"
#include "Audio.h"

int FMOD_Intergration::ErrorCheck ( FMOD_RESULT result )
{
    if ( result != FMOD_OK )
    {
        std::cout << "FMOD ERROR " << result << std::endl;
        return 1;
    }
    // std::cout << "FMOD all good" << std::endl;
    return 0;
}

FMOD_Intergration::FMOD_Intergration ( ) :
    mnNextChannelId( 0 )
{ 
    mpStudioSystem = NULL;
    ErrorCheck ( FMOD::Studio::System::create ( &mpStudioSystem ) );

    auto studioFlag = FMOD_STUDIO_INIT_NORMAL;
    auto coreFlag = FMOD_INIT_NORMAL ;// | FMOD_INIT_3D_RIGHTHANDED; // Open GL is a right handed system where as FMOD defaults to left handed.

#if _DEBUG
    studioFlag |= FMOD_STUDIO_INIT_LIVEUPDATE;
    coreFlag |= FMOD_INIT_PROFILE_ENABLE;
#endif

    ErrorCheck ( mpStudioSystem->initialize ( 32, studioFlag, coreFlag, NULL ) );

    mpSystem = NULL;
}

FMOD_Intergration::~FMOD_Intergration ( )
{ 
    ErrorCheck ( mpStudioSystem->unloadAll ( ) );
    ErrorCheck ( mpStudioSystem->release ( ) );
}

void FMOD_Intergration::Update ( )
{ 
    std::vector<ChannelMap::iterator> pStoppedChannels;
    
    for ( auto it = m_Channels.begin ( ), itEnd = m_Channels.end ( ); it != itEnd; ++it )
    {
        bool bIsPlaying = false;
        it->second->isPlaying ( &bIsPlaying );
        if ( !bIsPlaying )
        {
            pStoppedChannels.push_back ( it );
        }
    }

    for ( auto & it : pStoppedChannels )
    {
        m_Channels.erase ( it );
    }

    ErrorCheck ( mpStudioSystem->update ( ) );
}

