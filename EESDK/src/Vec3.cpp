///==========================================================================
/// \file	Vec3.cpp
/// \brief	
/// \date	08/10/2005
/// \author	Brian Bazyk
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
#include "Vec3.h"

IVec3Implementation *Vec3::impl = NULL;

namespace ElementalEngine
{

void* GetActiveMathImplementation( DWORD dwImplementationName );

int SetVec3Implementation()
{
	Vec3::impl = (IVec3Implementation*)GetActiveMathImplementation(CHashString(_T("Vec3DefaultImplementation")).GetUniqueID());
//	EngineGetToolBox()->Log( LOG_SEVERE, _T("Math implementation for Vec3 not found!\n") );
	return true;
}

} //namespace ElementalEngine

