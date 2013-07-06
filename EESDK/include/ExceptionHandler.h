///==========================================================================
/// \file	ExceptionHandler.h
/// \brief	C function for doing mini dump, stack dump, etc
/// \date	05/22/2007
/// \author	Nick Tourte
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
///==========================================================================

#ifndef _EXEPTIONHANDLER_H_
#define _EXEPTIONHANDLER_H_

int __cdecl HandleException(PEXCEPTION_POINTERS pExceptPtrs);

#endif