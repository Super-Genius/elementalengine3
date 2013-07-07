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
#include "CLoadSaveManager.hpp"
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
#define DLL_API extern "C" ELEMENTAL_DECLSPEC_EXPORT 
#endif

DLL_API void InitDLL()
{
	// on initialization, search for and load all
	// "*.dls" which are dynamic Loader/Saver modules in
	// dll format.
#ifndef _LIB
    SINGLETONINSTANCE(CLoadSaveManager)->LoadPlugins();
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
