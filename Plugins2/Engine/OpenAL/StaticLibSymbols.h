//Autogenerated Static Lib File:
//Generated on: 07/06/13 20:26:20

#ifndef INCLUDE_SYMBOLS_OPENAL
#define INCLUDE_SYMBOLS_OPENAL
#endif	//#ifndef INCLUDE_SYMBOLS_OPENAL
#ifdef _LIB

namespace ElementalEngine {
struct INITDLLINFO;
class CRegisterComponent;
class CRegisterMessage;
}

using namespace ElementalEngine;


extern void *OPENAL_LIBEXTERNS[];
//Singleton externs
extern "C" CRegisterComponent COpenALSoundUtilityRO;
//Component externs
extern "C" CRegisterComponent CMusicPlaylistRO;
extern "C" CRegisterComponent COpenALSoundObjectRO;
extern "C" CRegisterComponent COpenALSoundUtilityRO;
//Message externs
extern "C" CRegisterMessage PlaySoundObject_CSoundObjectRM;
extern "C" CRegisterMessage StopSoundObject_CSoundObjectRM;
extern "C" CRegisterMessage AddSound_COpenALSoundUtilityRM;
extern "C" CRegisterMessage SetListenerVectors_COpenALSoundUtilityRM;
extern "C" CRegisterMessage GetSoundFileInfo_COpenALSoundUtilityRM;
extern "C" CRegisterMessage ShutDownAudioEngine_COpenALSoundUtilityRM;
extern "C" CRegisterMessage LoadSoundDatabase_COpenALSoundUtilityRM;
extern "C" CRegisterMessage SetAudioBufferLength_COpenALSoundUtilityRM;
extern "C" CRegisterMessage SetAudioBufferCount_COpenALSoundUtilityRM;
extern "C" CRegisterMessage SetSoundScale_COpenALSoundUtilityRM;
extern "C" CRegisterMessage SetGlobalAttenuationMultiplier_COpenALSoundUtilityRM;
extern "C" CRegisterMessage SetGlobalDopplerFactor_COpenALSoundUtilityRM;
extern "C" CRegisterMessage UpdateAudioEngine_COpenALSoundUtilityRM;

#endif	//#ifdef _LIB
