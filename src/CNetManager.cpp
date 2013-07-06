///==========================================================================
/// \file	CNetManager.cpp
/// \brief	Network Manager
/// \date	4/1/2005
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

#include "StdAfx.h"

#include "CNetManager.h"

REGISTER_COMPONENT_SINGLETON( CNetManager );

REGISTER_MESSAGE_HANDLER(Start, OnStart, CNetManager);
REGISTER_MESSAGE_HANDLER(Stop, OnStop, CNetManager);

/// \brief	Default Destructor
///			Since CPhysManager is implemented as a singleton the
///			constructor is private.
CNetManager::CNetManager() :
	CManager(_T("CNetManager"))
{
	m_bIsRunning = false;
}

/// \brief	Default Destructor
CNetManager::~CNetManager()
{
}

/// \brief	Function to get this component.
///	\return	The singleton instance in this case
IComponent* CNetManager::Create(int nArgs, va_list argptr)
{
	// Return the singleton instance of this class
	return SINGLETONINSTANCE( CNetManager );
}

DWORD CNetManager::OnStart(DWORD size, void *data)
{
	m_bIsRunning = true;
	
	return MSG_HANDLED_PROCEED;
}

DWORD CNetManager::OnStop(DWORD size, void *data)
{
	m_bIsRunning = false;
	
	return MSG_HANDLED_PROCEED;
}
