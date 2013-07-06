///============================================================================
/// \file	CGLRendererDLL.cpp
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
#define GLRENDERER_PRIORITY 1000

void DeInitFontImplementation();
void InitFontImplementation();

#ifdef _LIB
#define DLL_API static
#else
#define DLL_API extern "C" ELEMENTAL_DECLSPEC_EXPORT
#endif

DLL_API void InitDLL()
{
}

DLL_API DWORD GetDLLVersion()
{
	return INTERFACEDLLVERSION;
}

DLL_API DWORD GetPriority()
{
	return GLRENDERER_PRIORITY;
}

DLL_API void DeInitDLL()
{
	IToolBox *toolBox;
	toolBox = EngineGetToolBox();

	SINGLETONINSTANCE( CGLRenderer )->OnDestroyRenderer();

	static DWORD msgHash_DestroyRenderer = CHashString(_T("DestroyRenderer")).GetUniqueID();
	toolBox->SendMessage(msgHash_DestroyRenderer, 0, NULL);

	SINGLETONINSTANCE( CGLRenderer )->DeInitialize();
}

#ifdef _LIB
#include ".\StaticLibSymbols.h"
INITDLLINFO GLRENDERER_INITDLLINFO = {
	InitDLL, 
	GetDLLVersion, 
	GetPriority, 
	DeInitDLL,
	GLRENDERER_LIBEXTERNS
};
#endif
