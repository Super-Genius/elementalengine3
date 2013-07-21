///============================================================================
/// \file	EEOSXLogWindowDLL
/// \brief	Defines the entry point for the DLL application.
/// \note   Elemental Engine
///         Copyright (C) 2013 Social Systems Technology, Inc.
///
///         This code is redistributable under the terms of the EE License.
///
///         This code is distributed without warranty or implied warranty of
///			merchantability or fitness for a particular purpose. See the
///			EE License for more details.
///
///         You should have received a copy of the EE License along with this
///			code; If not, write to Social Systems Technology, Inc.,
///			109 East 17th Street Suite 4210 Cheyenne, WY 82001 USA
///============================================================================

#include "stdafx.h"

#ifdef _DEBUG
#define INTERFACEDLLVERSION 0x00006100
#else
#define INTERFACEDLLVERSION 0x00006101
#endif

// we don't need loading before anybody, so set priority rather high
#define LOGVIEWER_PRIORITY 52001

#ifdef _LIB
#define DLL_API static
#else
#define DLL_API extern "C" ELEMENTAL_DECLSPEC_EXPORT
#endif


DLL_API void InitDLL()
{
    IToolBox *toolBox;
	toolBox = EngineGetToolBox();
    
//    CHashString clvcHash("CLogViewerComponent");
//	toolBox->CreateComponent(&clvcHash, 0);
}


DLL_API DWORD GetDLLVersion()
{
	return INTERFACEDLLVERSION;
}

DLL_API DWORD GetPriority()
{
	return LOGVIEWER_PRIORITY;
}

DLL_API void DeInitDLL()
{

}

#ifdef _LIB
#include ".\StaticLibSymbols.h"
INITDLLINFO GLRENDERER_INITDLLINFO = {
	InitDLL,
	GetDLLVersion,
	GetPriority,
	DeInitDLL,
	EEOSXLOGWINDOWDLL_LIBEXTERNS
};
#endif
