#pragma once

#include <FMOD/fmod.hpp>
#include <FMOD/fmod_studio.hpp>
#include <string>
#include <map>
#include <vector>
#include <math.h>
#include <iostream>
#include <glm\glm.hpp>

class FMOD_Intergration
{
    friend class Audio;

public:

    static int ErrorCheck ( FMOD_RESULT result );

private:

    FMOD_Intergration ( );
    ~FMOD_Intergration ( );

    void Update ( );

    FMOD::Studio::System * mpStudioSystem;
    FMOD::System * mpSystem;

    int mnNextChannelId;

    typedef std::map<std::string, FMOD::Sound *> SoundMap;
    typedef std::map<int, FMOD::Channel *> ChannelMap;
    typedef std::map<std::string, FMOD::Studio::EventInstance *> EventMap;
    typedef std::map<std::string, FMOD::Studio::Bank *> BankMap;

    BankMap m_Banks;
    EventMap m_Events;
    SoundMap m_Sounds;
    ChannelMap m_Channels;
};