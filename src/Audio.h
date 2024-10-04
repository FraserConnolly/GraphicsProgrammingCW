/*
* This and FMOD.h are adapted from 
* https://codyclaborn.me/tutorials/making-a-basic-fmod-audio-engine-in-c/
* The reference material is for an older version of FMOD so quite a few adjustments were needed.
* Also the original author had quite a few functions declared but not defined.
*/

#pragma once

#include <FMOD/fmod.hpp>
#include <FMOD/fmod_studio.hpp>
#include <string>
#include <map>
#include <vector>
#include <math.h>
#include <iostream>
#include <glm\glm.hpp>

class FMOD_Intergration;
class Transform; 

class Audio
{
public:
    static void Startup ( );
    static void Service ( );
    static void Shutdown ( );

    static void LoadBank ( const std::string & strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags );
    
    static void Set3dListenerAndOrientation ( const int listenerIndex, const Transform & transform );
    static void Set3dListenerAndOrientation ( const int listenerIndex, const glm::vec3 & vPosition, const glm::vec3 & vForward, const glm::vec3 & vUp );

#pragma region Sounds
    
    // All sound functions untested as they aren't used in this project.
    // Instead this project uses events which are defined in FMOD Studio.

    static void LoadSound ( const std::string & strSoundName, bool b3d = true, bool bLooping = false, bool bStream = false );
    static void UnLoadSound ( const std::string & strSoundName );
    /// <summary>
    /// Plays an audio file.
    /// </summary>
    /// <param name="strSoundName">File path</param>
    /// <param name="vPosition"></param>
    /// <param name="fVolumedB"></param>
    /// <returns>Channel number</returns>
    static int PlaySound ( const std::string & strSoundName, const glm::vec3 & vPosition = glm::vec3 { 0, 0, 0 }, float fVolumedB = 0.0f );
    static void StopChannel ( int nChannelId );
    static void StopAllChannels ( );
    static void SetChannel3dPosition ( int nChannelId, const glm::vec3 & vPosition );
    static void SetChannelVolume ( int nChannelId, float fVolumedB );
    static bool IsPlaying ( int nChannelId );

#pragma endregion

#pragma region FMOD Events

    static void LoadEvent ( const std::string & strEventName );
    static void SetEvent3dAttributes ( const std::string & strEventName, const Transform & transform );
    static void SetEvent3dAttributes ( const std::string & strEventName, const glm::vec3 & vPosition );
    static void PlayEvent ( const std::string & strEventName );
    static void StopEvent ( const std::string & strEventName, bool bImmediate = false );
    static void GetEventParameter ( const std::string & strEventName, const std::string & strEventParameter, float & parameter );
    static void SetEventParameter ( const std::string & strEventName, const std::string & strParameterName, float fValue );
    static bool IsEventPlaying ( const std::string & strEventName );

#pragma endregion

#pragma region Helpers

    static float dbToVolume ( float db );
    static float VolumeTodb ( float volume );
    static FMOD_VECTOR VectorToFmod ( const glm::vec3 & vPosition );

#pragma endregion

private:
    
    static FMOD_Intergration * s_pFmodIntegration;
};