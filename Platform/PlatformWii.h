
#ifndef PLATFORMWII_H
#define PLATFORMWII_H

#define _T(x) x

namespace Metrowerks {
};
using namespace Metrowerks;

#define _tcscat     strcat
#define _tcscpy     strcpy
#define _tcsdup     strdup
#define _tcslen     strlen
#define _tcsnlen    strnlen
#define _tcsxfrm    strxfrm
#define _tcserror   strerror
#define __tcserror  strerror

#define _tcschr         strchr
#define _tcscspn        strcspn
#define _tcsncat        strncat
#define _tcsncpy        strncpy
#define _tcspbrk        strpbrk
#define _tcsrchr        strrchr
#define _tcsspn         strspn
#define _tcsstr         strstr
#define _tcstok         strtok
#define _tcstok_s       strtok_s
#define _tcsninc(p, l)	(p + l)

#define _tcsnset        strnset
#define _tcsrev         strrev
#define _tcsset         strset

#define _tcscmp         strcmp
#define _tcsicmp        stricmp
#define _tcsnccmp       strncmp
#define _tcsncmp        strncmp
#define _tcsnicmp       strnicmp
#define _tcsncicmp      strnicmp
#define _tcslwr			strlwr
#define _tcscoll        strcoll
#define _tcsicoll       stricoll

#define _vsnprintf vsnprintf
#define _snprintf snprintf

#define _tsplitpath     _splitpath
#define _stprintf       sprintf
#define _stscanf        sscanf
#define _tstat			stat

#define STATSTRUCT struct stat

#define _ltot			ltoa
#define _itot			itoa
#define _tstol			atol

#define __forceinline __inline__
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef unsigned int UINT;
typedef unsigned int UINT32;
typedef int DWORD;
typedef int INT;
typedef unsigned int HWND;
typedef unsigned int HMODULE;
typedef unsigned int HINSTANCE;
typedef unsigned int HANDLE;
typedef unsigned int HACCEL;
typedef void *	HGLOBAL;
typedef long HRESULT;
typedef long LONG;
typedef unsigned long ULONG;
typedef long long LONGLONG;
typedef unsigned long long ULONGLONG;
typedef unsigned long long UINT64;
typedef long long INT64;
#ifdef  _UNICODE
#include <wchar.h>
typedef wchar_t TCHAR;
typedef wchar_t *PCHAR;
#else
typedef char TCHAR;
typedef char *PCHAR;
#endif

#define LOWORD(l)           ((WORD)((DWORD)(l) & 0xffff))
#define HIWORD(l)           ((WORD)((DWORD)(l) >> 16))
#define LOBYTE(w)           ((BYTE)((DWORD)(w) & 0xff))
#define HIBYTE(w)           ((BYTE)((DWORD)(w) >> 8))

#define _FREE_BLOCK		0
#define _NORMAL_BLOCK	1
#define _CRT_BLOCK		2
#define _IGNORE_BLOCK	3
#define _CLIENT_BLOCK	4

typedef union _LARGE_INTEGER {
    struct {
        DWORD LowPart;
        LONG HighPart;
    };
    struct {
        DWORD LowPart;
        LONG HighPart;
    } u;
    LONGLONG QuadPart;
} LARGE_INTEGER;

typedef const TCHAR * LPCTSTR;
typedef TCHAR *LPTSTR;
typedef BYTE *LPBYTE;
// maximum number of exception parameters
#define EXCEPTION_MAXIMUM_PARAMETERS 15

typedef struct _EXCEPTION_RECORD32 {
    DWORD ExceptionCode;
    DWORD ExceptionFlags;
    DWORD ExceptionRecord;
    DWORD ExceptionAddress;
    DWORD NumberParameters;
    DWORD ExceptionInformation[EXCEPTION_MAXIMUM_PARAMETERS];
} EXCEPTION_RECORD, *PEXCEPTION_RECORD;

typedef struct _EXCEPTION_POINTERS {
	PEXCEPTION_RECORD *ExceptionRecord;
} EXCEPTION_POINTERS, *PEXCEPTION_POINTERS;

#define _isnan isnan
#define _finite isfinite

#define __cdecl

#define _atoi64 atoll
#define _tstof atof
#define _tstoi atoi

#define CONST const

typedef struct tagRECT
{
    LONG    left;
    LONG    top;
    LONG    right;
    LONG    bottom;
} RECT;

#include "os.h"

#define QueryPerformanceFrequency(x) (((x)->QuadPart = (LONGLONG)OS_TIMER_CLOCK) == CLOCKS_PER_SEC)
#define QueryPerformanceCounter(x) ((x)->QuadPart = OSGetTime())
#define GetTickCount OSGetTime

#define MAX_PATH 1024
#define _MAX_DRIVE 1024
#define _MAX_DIR 1024
#define _MAX_FNAME 1024
#define _MAX_EXT 1024

extern "C" void _splitpath (const TCHAR *path, TCHAR *drive, TCHAR *dir, TCHAR *fname, TCHAR *ext);

// until we figure out debugging output on Wii.
#define OutputDebugString(x)
#define __debugbreak()
#define DebugBreak()

#define GLOBALALLOC(x) malloc(x)
#define GLOBALFREE(x) free(x)

typedef struct tPOINT
{
    LONG  x;
    LONG  y;
} POINT;

// crt memory functions
#define _CrtMemCheckpoint(x) 
#define _CrtMemDumpAllObjectsSince(x)
typedef struct _CrtMemState {} _CrtMemState;

extern "C" INT CreateBannerSaveFile(INT flag, const WORD *title, const WORD *comment, const BYTE *bannerTexture, const BYTE **iconsTexture, BYTE numOfIcons);

#endif // #ifndef PLATFORMWII_H

