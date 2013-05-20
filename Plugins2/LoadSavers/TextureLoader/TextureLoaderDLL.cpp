///============================================================================
/// \note   Elemental Engine
///         Copyright (c)  2005-2008 Signature Devices, Inc.
///
///         This code is redistributable under the terms of the EE License.
///
///         This code is distributed without warranty or implied warranty of
///			merchantability or fitness for a particular purpose. See the 
///			EE License for more details.
///
///         You should have received a copy of the EE License along with this
///			code; If not, write to Signature Devices, Inc.,
///			3200 Bridge Parkway Suite 102, Redwood City, CA 94086 USA.
///============================================================================

#include "stdafx.h"

#include "EEDefines.h"

#ifdef _DEBUG
#define INTERFACEDLLVERSION 0x00006100
#else
#define INTERFACEDLLVERSION 0x00006101
#endif

#define LOADERSAVER_PRIORITY 15451

#ifdef _LIB
#define DLL_API static
#else
#define DLL_API extern "C" __declspec(dllexport)
#endif

DLL_API void InitDLL()
{
	IToolBox *toolBox = EngineGetToolBox();

	static CHashString CTextureLoaderType(_T("CTextureLoader"));
	toolBox->CreateComponent(&CTextureLoaderType, 0);}

DLL_API DWORD GetDLLVersion()
{
	return INTERFACEDLLVERSION;
}

DLL_API DWORD GetPriority()
{
	return LOADERSAVER_PRIORITY;
}

#ifdef _LIB

#include ".\StaticLibSymbols.h"

INITDLLINFO TEXTURELOADER_INITDLLINFO = {
	InitDLL, 
	GetDLLVersion, 
	GetPriority, 
	NULL,
	TEXTURELOADER_LIBEXTERNS
};

#else


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			break;
	}
    return TRUE;
}

#endif