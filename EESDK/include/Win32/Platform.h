#ifndef PLATFORMWIN32_H__
#define PLATFORMWIN32_H__

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500 
#endif //_WIN32_WINNT

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <assert.h>
#include <direct.h>

#ifdef _DEBUG
// This define must occur before any headers are included.
#include <crtdbg.h>
#ifndef DEBUG_NEW
#define DEBUG_NEW new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#endif
#endif

// comes from ElementalEngine.hpp
#define GLOBALALLOC(size) GlobalAlloc(GMEM_FIXED, size)
#define GLOBALFREE(x) GlobalFree((HGLOBAL)x)

#define STATSTRUCT struct _stat

typedef WIN32_FIND_DATA FIND_DATA;

#define EE_ENDIANSWAP32( i ) (i)
#define EE_ENDIANSWAP32F( f ) (f)
#define EE_ENDIANSWAP16( i ) (i)

// has to be used because of hash_map is in stdext namespace
#ifdef __cplusplus
#ifdef _MSC_VER
namespace stdext{}
using namespace stdext; 
#include <hash_map>
#endif
#endif

#endif // PLATFORM_WIN32_H__

