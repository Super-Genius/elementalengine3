///============================================================================
/// \file		DLLVersion.h
/// \date		07-14-2005
/// \author		Kenneth Hurley
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

#ifndef _DLLVERSION_H_
#define _DLLVERSION_H_



#ifdef _DEBUG
// major version in upper 16 bits
// low word =  minor version, low bit debug =0, release=1
#define INTERFACEDLLVERSION 0x00006100
#else
#define INTERFACEDLLVERSION 0x00006101
#endif

// plugin exported functions
typedef void  (*INITDLL)();
typedef DWORD (*GETDLLVERSION)();
typedef DWORD (*GETPRIORITY)();
typedef void (*DEINITDLL)();

#endif // #ifndef _DLLVERSION_H_

#ifdef DLLIMPORTING
extern "C" ELEMENTAL_DECLSPEC_IMPORT DWORD CDECL GetDLLVersion();
#else
extern "C" ELEMENTAL_DECLSPEC_EXPORT DWORD CDECL GetDLLVersion();
#endif


