///==========================================================================
/// \file	Matrix4x4.cpp
/// \brief	
/// \date	08/02/2006
/// \author	Kyle Swaim
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

#include "stdafx.h"
#include "Matrix4x4.h"

IMatrix4x4Implementation *Matrix4x4::impl = NULL;

namespace ElementalEngine
{

void* GetActiveMathImplementation( DWORD dwImplementationName );

int SetMatrix4x4Implementation()
{
	Matrix4x4::impl = (IMatrix4x4Implementation*)GetActiveMathImplementation(CHashString(_T("Matrix4x4DefaultImplementation")).GetUniqueID());
	return true;
}

} //namespace ElementalEngine

