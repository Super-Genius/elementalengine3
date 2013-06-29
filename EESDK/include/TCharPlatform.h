
// header file tchar replacement
#ifndef TCHARPLATFORM_H
#define TCHARPLATFORM_H

#ifdef  _MSC_VER
#include <tchar.h>
#else

#ifdef  _UNICODE
#include <wchar.h>
typedef wchar_t TCHAR;
#else
typedef char TCHAR;

#endif

#endif	// #ifndef TCHARPLATFORM_H