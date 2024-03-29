//Autogenerated Static Lib File:
//Generated on: 07/06/13 20:26:04

#ifndef INCLUDE_SYMBOLS_DX9RENDERER
#define INCLUDE_SYMBOLS_DX9RENDERER
#endif	//#ifndef INCLUDE_SYMBOLS_DX9RENDERER
#ifdef _LIB

namespace ElementalEngine {
struct INITDLLINFO;
class CRegisterComponent;
class CRegisterMessage;
}

using namespace ElementalEngine;


extern void *DX9RENDERER_LIBEXTERNS[];
//Singleton externs
extern "C" CRegisterComponent CMoviePlayerRO;
extern "C" CRegisterComponent CDX9RendererRO;
extern "C" CRegisterComponent CHModelManagerRO;
//Component externs
extern "C" CRegisterComponent CDX9CubeTextureObjectRO;
extern "C" CRegisterComponent CMoviePlayerRO;
extern "C" CRegisterComponent CPixelShaderRO;
extern "C" CRegisterComponent CDX9RendererRO;
extern "C" CRegisterComponent CDX9TextureObjectRO;
extern "C" CRegisterComponent CVertexShaderRO;
extern "C" CRegisterComponent CHModelManagerRO;
//Message externs
extern "C" CRegisterMessage PlayMovie_CMoviePlayerRM;
extern "C" CRegisterMessage StopMovie_CMoviePlayerRM;
extern "C" CRegisterMessage PauseMovie_CMoviePlayerRM;
extern "C" CRegisterMessage SetMovieVolume_CMoviePlayerRM;
extern "C" CRegisterMessage IsMovieDone_CMoviePlayerRM;
extern "C" CRegisterMessage DestroyRenderer_CMoviePlayerRM;
extern "C" CRegisterMessage GetRendererInterface_CDX9RendererRM;
extern "C" CRegisterMessage SetDirectories_CDX9RendererRM;
extern "C" CRegisterMessage GetRayFromMouseCoords_CDX9RendererRM;
extern "C" CRegisterMessage TransformWorldToScreen_CDX9RendererRM;

#endif	//#ifdef _LIB
