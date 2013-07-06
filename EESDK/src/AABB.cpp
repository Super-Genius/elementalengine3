///==========================================================================
/// \file	AABB.cpp
/// \brief	
/// \date	08/02/2006
/// \author	Marvin Gouw
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
#include "AABB.h"

IAABBImplementation *AABB::impl = NULL;

namespace ElementalEngine
{

void* GetActiveMathImplementation( DWORD dwImplementationName );

int SetAABBImplementation()
{
	AABB::impl = (IAABBImplementation*)GetActiveMathImplementation(CHashString(_T("AABBDefaultImplementation")).GetUniqueID());
	return true;
}

} //namespace ElementalEngine

