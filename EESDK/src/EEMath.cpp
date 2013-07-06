///==========================================================================
/// \file	MathUtils.cpp
/// \brief	Math Utilities
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

namespace ElementalEngine
{

int InitializeMathImplementations()
{
	SetVec3Implementation();
	SetQuaternionImplementation();
	SetMatrix3x3Implementation();
	SetMatrix4x4Implementation();
	SetMathUtilImplementation();
	SetPlaneImplementation();
	SetRandomImplementation();
	SetEulerImplementation();
	SetAABBImplementation();
	return true;
}

extern "C" {
int MathImplementationsRegistered = InitializeMathImplementations();
}

void* GetActiveMathImplementation( DWORD dwImplementationName )
{
	ACTIVEMATHIMPLEMENTATION msg;
	msg.dwImplementationName = dwImplementationName;
	static DWORD msgHash_GetActiveMathImplementation = CHashString(_T("GetActiveMathImplementation")).GetUniqueID();
	if (EngineGetToolBox()->SendMessage(msgHash_GetActiveMathImplementation, sizeof(msg), &msg ) != MSG_HANDLED)
		return NULL;
	else
		return msg.pImplementation;
}

} //namespace ElementalEngine

