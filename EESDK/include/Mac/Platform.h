#ifndef _PLATFORM_MAC_H_
#define _PLATFORM_MAC_H_

#include <math.h>
#include <sys/time.h>
#include <mach/mach_time.h>
#include <string.h>
#include <errno.h>
#ifdef __cplusplus
#include <ext/hash_map>
using namespace __gnu_cxx;
#endif
#include <wchar.h>

//Missing typedef for MAC
typedef char TCHAR;
typedef unsigned int UINT;
typedef int INT;
typedef unsigned long DWORD;
typedef unsigned long ULONG;
typedef __int64_t LONGLONG;
typedef uint64_t ULONGLONG;
typedef int BOOL;
typedef uint64_t UINT64;
typedef LONGLONG LARGE_INTEGER;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef long HRESULT;
typedef uint64_t ULONG_PTR;
typedef ULONG_PTR DWORD_PTR, *PDWORD_PTR;

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
//GCC have no support for novtable
#define __declspec(novtable)
#define PEXCEPTION_POINTERS void*
#define __cdecl


#define _finite(n) finite(n)
#define _isnan(n) isnan(n)
#ifndef FALSE
#define FALSE  0
#endif

#define HMODULE DWORD
#define HANDLE DWORD
#define HWND  DWORD
#define HINSTANCE DWORD
#define CONST const
#define __forceinline __inline
#define QueryPerformanceFrequency GetTimeInNsSinceCPUStart
#define MAX_PATH 260
#define _MAX_DRIVE 3
#define _MAX_DIR 256
#define _MAX_FNAME 256
#define _MAX_EXT 256
#define _MAX_PATH MAX_PATH


typedef struct _FIND_DATA
{
    
} FIND_DATA;

#define EE_ENDIANSWAP32( i ) (i)
#define EE_ENDIANSWAP32F( f ) (f)
#define EE_ENDIANSWAP16( i ) (i)

//
// This is a replacement for QueryPerformanceFrequency / QueryPerformanceCounter
// returns nanoseconds since system start
//
UINT64 GetTimeInNsSinceCPUStart()
{
	double time;

	time = mach_absolute_time();

	// this is the timebase info
    static mach_timebase_info_data_t info;
    mach_timebase_info(&info);
    double nano = ( (double) info.numer) / ((double) info.denom);

	return (UINT64) nano * time / 1000000000.0;
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
void _splitpath(const char* inpath, char * drv, char * dir,
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

#endif

