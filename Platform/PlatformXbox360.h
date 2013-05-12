
#include <xtl.h>
#include <xboxmath.h>
#include <tchar.h>

#define _XBOX_CRT_DEPRECATE_INSECURE

// Missing D3D defines
#ifndef D3DUSAGE_AUTOGENMIPMAP
#define D3DUSAGE_AUTOGENMIPMAP 0
#endif

#ifndef D3DUSAGE_DYNAMIC
#define D3DUSAGE_DYNAMIC 0
#endif

#ifndef D3DLOCK_DISCARD
#define D3DLOCK_DISCARD 0
#endif

#ifndef D3DSGR_CALIBRATE
#define D3DSGR_CALIBRATE 0
#endif

#ifndef D3DDECLUSAGE_POSITIONT
#define D3DDECLUSAGE_POSITIONT D3DDECLUSAGE_POSITION
#endif

// Missing color functions
#ifdef XBOX
#ifndef RGB
#define RGB(r,g,b)          ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))
#endif
#ifndef GetRValue
#define GetRValue(rgb)      (LOBYTE(rgb))
#endif
#ifndef GetGValue
#define GetGValue(rgb)      (LOBYTE(((WORD)(rgb)) >> 8))
#endif
#ifndef GetBValue
#define GetBValue(rgb)      (LOBYTE((rgb)>>16))
#endif
#endif

// EE compiler defines
#define DISABLE_RANGED_BUFFER_LOCKS
#define DISABLE_FIXEDFUNCTION

#define EE_BIG_ENDIAN

#define EEXBOX_SETTHREADCORE( thread, core ) { XSetThreadProcessor( thread, core ); }

#ifdef _DEBUG
// This define must occur before any headers are included.
//#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK, THIS_FILE, __LINE__)

//Comes from ElementalEnging.hpp
#define GLOBALALLOC malloc
#define GLOBALFREE free

#define STATSTRUCT struct _stat


#endif
