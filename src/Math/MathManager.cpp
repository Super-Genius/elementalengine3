///==========================================================================
/// \file	MathManager.cpp
/// \brief	Implementation of Math Manager
/// \date	08-12-2005
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

#include "StdAfx.h"
#ifndef _LIB
#include "DLLVersion.h"
#endif
#include "MathManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace ElementalEngine
{
extern void	InitDefaultMathImplementation();

REGISTER_COMPONENT_SINGLETON( CMathManager );
REGISTER_MESSAGE_HANDLER(GetActiveMathImplementation, OnGetActiveMathImplementation, CMathManager);
REGISTER_MESSAGE_HANDLER(RegisterMathImplementation, OnRegisterMathImplementation, CMathManager);

IComponent *CMathManager::Create(int nArgs, va_list argptr)
{
	return SINGLETONINSTANCE( CMathManager );
}

CMathManager::CMathManager() : 
	CManager(_T("CMathManager"))
{
	m_iCurrentMathLibraryPriority = 9999999;
#ifndef _LIB
	LoadDLMs();
#endif
}

CMathManager::~CMathManager()
{
#ifndef _LIB
	m_ToolBox->UnloadPlugins(m_DLLPriorityMap);
#endif
}

DWORD CMathManager::OnGetActiveMathImplementation(DWORD size, void *in_params)
{
	VERIFY_MESSAGE_SIZE(size, sizeof(ACTIVEMATHIMPLEMENTATION));
	ACTIVEMATHIMPLEMENTATION *pParams = (ACTIVEMATHIMPLEMENTATION*)in_params;

	pParams->pImplementation = NULL;

	// get current math library
	MATHLIBRARYMAP::iterator itrMathLib = m_MathLibraryMap.find( m_dwCurrentMathLibrary );
	if (itrMathLib == m_MathLibraryMap.end())
		return MSG_ERROR; // current math library not found (logic error, should never happen)

	// get implementation map
	MATHIMPLEMENTATIONMAP *pImplementationMap = &itrMathLib->second;

	// get implementation
	MATHIMPLEMENTATIONMAP::iterator itrMathImp = pImplementationMap->find( pParams->dwImplementationName );
	if (itrMathImp == pImplementationMap->end())
		return MSG_ERROR; // requested implementation not supported by current math library

	// success
	pParams->pImplementation = itrMathImp->second;

	return MSG_HANDLED_STOP;
}

DWORD CMathManager::OnRegisterMathImplementation(DWORD size, void *in_params)
{
	VERIFY_MESSAGE_SIZE(size, sizeof(REGISTERMATHIMPLEMENTATION));
	REGISTERMATHIMPLEMENTATION *pParams = (REGISTERMATHIMPLEMENTATION*)in_params;

	MATHIMPLEMENTATIONMAP *pDestImpMap = &m_MathLibraryMap[pParams->dwLibraryName];

	// merge registeree's map of implementations with the stored libraries map
	// of implementations
	// TODO: use a multimap and store individual implementation priorities and 
	// only overwrite an implementation if registeree's map is higher priority
	MATHIMPLEMENTATIONMAP::iterator itr = pParams->pImplementationMap->begin();
	while (itr != pParams->pImplementationMap->end())
	{
		(*pDestImpMap)[itr->first] = itr->second;
		itr++;
	}

	if (pParams->iPriority < m_iCurrentMathLibraryPriority)
	{
		m_dwCurrentMathLibrary = pParams->dwLibraryName;
		m_iCurrentMathLibraryPriority = pParams->iPriority;
	}

	return MSG_HANDLED_STOP;
}

void CMathManager::LoadDLMs()
{
#ifndef _LIB
	m_ToolBox->LoadPlugins(_T(".\\Plugins\\*.dlm"), m_DLLPriorityMap);
	m_ToolBox->InitPlugins(m_DLLPriorityMap);
#endif
	if (m_DLLPriorityMap.size() == 0)
	{
		InitDefaultMathImplementation();
	}
}

}