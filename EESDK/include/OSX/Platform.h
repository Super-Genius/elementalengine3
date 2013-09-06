#ifndef _PLATFORM_MAC_H_
#define _PLATFORM_MAC_H_

#include <math.h>
#include <sys/time.h>
#include <mach/mach_time.h>
#include <string.h>
#include <errno.h>
#include <wchar.h>
#include <errno.h>
#include <syslog.h>

#ifdef __cplusplus
#include <ext/hash_map>
using namespace __gnu_cxx;
#endif

#define ELEMENTAL_DECLSPEC
#define ELEMENTAL_DECLSPEC_IMPORT
#define ELEMENTAL_DECLSPEC_EXPORT

#ifndef APIENTRY
#define APIENTRY
#endif

#define HAVE_STDINT_H

//Missing typedefs for MAC
typedef char TCHAR;
typedef unsigned int UINT;
typedef int INT;
typedef unsigned long DWORD;
typedef unsigned long ULONG;
typedef __int64_t LONGLONG;
typedef uint64_t ULONGLONG;
typedef int BOOL;
typedef uint64_t UINT64;
typedef int64_t INT64;
typedef LONGLONG LARGE_INTEGER;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef long HRESULT;
typedef uint64_t ULONG_PTR;
typedef ULONG_PTR DWORD_PTR, *PDWORD_PTR;
typedef const TCHAR * LPCTSTR;
typedef TCHAR * LPTSTR;
typedef void * LPVOID;

#ifdef DEBUG
#ifndef DEBUG_NEW
#define DEBUG_NEW new
#endif
#endif

#define LOWORD(l)           ((WORD)((DWORD_PTR)(l) & 0xffff))
#define HIWORD(l)           ((WORD)((DWORD_PTR)(l) >> 16))
#define LOBYTE(w)           ((BYTE)((DWORD_PTR)(w) & 0xff))
#define HIBYTE(w)           ((BYTE)((DWORD_PTR)(w) >> 8))
//#define new(_NORMAL_BLOCK, THIS_FILE, __LINE__)

//Some missing defines for iPhone
#define _T(x)  x
#define _tcscmp  strcmp
#define _tcslen  strlen
#define _tcscpy  strcpy
#define _tcsstr  strstr
#define _tcscat  strcat
#define _tcsdup  strdup
#define _tsplitpath _splitpath
#define _tcsclen strlen
#define GetTickCount mach_absolute_time
#define _atoi64 atoll
#define _stat stat
#define _tstat stat
#define _tstof atof
#define _tcscspn strcspn
#define _tcsncmp strncmp
#define _strninc(_str, _inc) (((_str)+(_inc)))
#define _tcsninc _strninc
#define _stprintf sprintf
#define _tstoi atoi
#define _ttoi atoi
#define	_stscanf sscanf
#define _vsnprintf vsnprintf
#define _snprintf snprintf
#define _tcsncicmp strncasecmp
#define _tcsnicmp strncasecmp
#define _tcsicmp strcasecmp
#define _tcslwr strlwr
#define _tcsupr strupr
#define _tcschr strchr
#define _tcsrchr strrchr

#define INVALID_HANDLE_VALUE (NULL)

//GCC have no support for novtable
#define PEXCEPTION_POINTERS void*
#define __cdecl

typedef struct stat STATSTRUCT;

#define CDECL __attribute__((cdecl))

#define OutputDebugString(fmt, ...) printf(fmt, ##__VA_ARGS__); \
                                    syslog(LOG_DEBUG, fmt, ##__VA_ARGS__)

#define _finite(n) finite(n)
#define _isnan(n) isnan(n)
#ifndef FALSE
#define FALSE  0
#endif

typedef void * HMODULE;
typedef void * HANDLE;
typedef void * HWND;
typedef void * HINSTANCE;
#define CONST const
#define __forceinline __inline
#define QueryPerformanceFrequency GetMachineFrequencyTPS
#define QueryPerformanceCounter GetTimeInNsSinceCPUStart
#define MAX_PATH 4096
#define _MAX_DRIVE 3
#define _MAX_DIR MAX_PATH
#define _MAX_FNAME MAX_PATH
#define _MAX_EXT MAX_PATH
#define _MAX_PATH MAX_PATH

typedef struct _FIND_DATA
{
    TCHAR cFileName[MAX_PATH+1];
} FIND_DATA;

#define ERROR_SUCCESS 0
#define ERROR_INVALID_FUNCTION -1
#define ERROR_INVALID_PARAMETER -2

#define GetLastError() errno

#define EE_ENDIANSWAP32( i ) (i)
#define EE_ENDIANSWAP32F( f ) (f)
#define EE_ENDIANSWAP16( i ) (i)

//
// This is a replacement for QueryPerformanceFrequency / QueryPerformanceCounter
// returns nanoseconds since system start
//
inline BOOL GetMachineFrequencyTPS(LARGE_INTEGER *ticksPerSecond)
{
    if (ticksPerSecond != NULL)
    {
        *ticksPerSecond = 1000000000;
        return TRUE;
    }
    
    return FALSE;
}

inline BOOL GetTimeInNsSinceCPUStart(LARGE_INTEGER *ticksPerSecond)
{
	double time;

	time = mach_absolute_time();

	// this is the timebase info
    static mach_timebase_info_data_t info;
    mach_timebase_info(&info);
    double nano = ( (double) info.numer) / ((double) info.denom);

    if (ticksPerSecond != NULL)
    {
        *ticksPerSecond = (UINT64) nano * time / 1000000000.0;
        return TRUE;
    }
    
    return FALSE;
}


/*********************************************************************
*              _splitpath
*
* Split a path into its component pieces.
*
* PARAMS
*  inpath [I] Path to split
*  drv    [O] Destination for drive component (e.g. "A:"). Must be at least 3 characters.
*  dir    [O] Destination for directory component. Should be at least MAX_PATH characters.
*  fname  [O] Destination for File name component. Should be at least MAX_PATH characters.
*  ext    [O] Destination for file extension component. Should be at least MAX_PATH characters.
*
* RETURNS
*  Nothing.
*/
inline void _splitpath(const char* inpath, char * drv, char * dir,
                         char* fname, char * ext )
{
     const char *p, *end;

     if (inpath[0] && inpath[1] == ':')
     {
         if (drv)
         {
             drv[0] = inpath[0];
             drv[1] = inpath[1];
             drv[2] = 0;
         }
         inpath += 2;
     }
     else if (drv) drv[0] = 0;

     /* look for end of directory part */
     end = NULL;
     for (p = inpath; *p; p++)
     {
         if (*p == '/' || *p == '\\')
         {
             end = p + 1;
         }
     }

     if (end)  /* got a directory */
     {
         if (dir)
         {
             memcpy( dir, inpath, end - inpath );
             dir[end - inpath] = 0;
         }
         inpath = end;
     }
     else if (dir)
     {
         dir[0] = 0;
     }

     /* look for extension: what's after the last dot */
     end = NULL;
     for (p = inpath; *p; p++)
     {
         if (*p == '.')
             end = p;
   
     }
     if (!end)
         end = p; /* there's no extension */

     if (fname)
     {
         memcpy( fname, inpath, end - inpath );
         fname[end - inpath] = 0;
     }
     if (ext)
         strcpy( ext, end );
}

inline char *strlwr (char *text)
{
    char *origtext = text;
    while (*text != '\0')
    {
        if ((*text > 64) && (*text < 91)) *text += 32;
        text++;
    }
    return (origtext);
}

inline char *strupr (char *text)
{
    char *origtext = text;
    while (*text != '\0')
    {
        if ((*text > 96) && (*text < 123)) *text -= 32;
        text++;
    }
    return (origtext);
}


#endif

