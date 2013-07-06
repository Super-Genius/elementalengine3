///=====================================================================
/// \file	CDelayedDeleteListener.cpp
///         This class implements a CDelayedDeleteListener which is a 
///			listener registered with object factory
/// \date	09/14/2005
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
///=====================================================================

#include "StdAfx.h"
#include "CObjectFactory.h"
#include "CDelayedDeleteListener.h"

// constructor for the listener class
CDelayedDeleteListener::CDelayedDeleteListener(CObjectFactory *parent)
{
	m_Parent = parent;
}

// function called when the listener is initialized from
// the system listener
bool CDelayedDeleteListener::Init(void)
{
	return true;
}

// Update function called from the system listener
void CDelayedDeleteListener::Update(DWORD tickCount)
{
	m_Parent->DeleteDelayedObjects();
}

// call from system listener to shutdown
bool CDelayedDeleteListener::DeInit(void)
{
	return true;
}

// returns name of manager type
IHashString* CDelayedDeleteListener::GetName()
{
	if (m_Parent)
	{
		return m_Parent->GetComponentType();
	}
	else
	{
		return NULL;	
	}
}