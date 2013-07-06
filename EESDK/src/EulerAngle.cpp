///==========================================================================
/// \file	EulerAngle.cpp
/// \brief	
/// \date	06/16/2007
/// \author	Marvin Gouw
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
#include "EulerAngle.h"

IEulerImplementation *EulerAngle::impl = NULL;

namespace ElementalEngine
{

void* GetActiveMathImplementation( DWORD dwImplementationName );

int SetEulerImplementation()
{
	EulerAngle::impl = (IEulerImplementation*)GetActiveMathImplementation(CHashString(_T("EulerDefaultImplementation")).GetUniqueID());
	return true;
}

} //namespace ElementalEngine

