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

#include "StdAfx.h"
#include <string>
using namespace std;

#ifdef _DEBUG
#define INTERFACEDLLVERSION 0x00006100
#else
#define INTERFACEDLLVERSION 0x00006101
#endif

// we don't need loading before anybody, so set priority rather high
#define LOADERSAVERMANAGER_PRIORITY 15450


#ifdef _LIB
#define DLL_API static
#else
#define DLL_API extern "C" __declspec(dllexport)

static DLLPRIORITYMAP gDLLPMap;

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			break;
		case DLL_PROCESS_DETACH:
			EngineGetToolBox()->UnloadPlugins(gDLLPMap);
			break;
	}

    return TRUE;
}
 
#endif

DLL_API void InitDLL()
{
	// on initialization, search for and load all
	// "*.dls" which are dynamic Loader/Saver modules in
	// dll format.
#ifndef _LIB
	EngineGetToolBox()->LoadPlugins(_T(".\\Plugins\\*.dls"), gDLLPMap);
	EngineGetToolBox()->InitPlugins(gDLLPMap);
#endif
}

DLL_API DWORD GetDLLVersion()
{
	return INTERFACEDLLVERSION;
}

DLL_API DWORD GetPriority()
{
	return LOADERSAVERMANAGER_PRIORITY;
}

#ifdef _LIB
#include ".\StaticLibSymbols.h"
INITDLLINFO LOADSAVEMANAGER_INITDLLINFO = {
	InitDLL, 
	GetDLLVersion, 
	GetPriority, 
	NULL,
	LOADSAVEMANAGER_LIBEXTERNS
};
#endif
