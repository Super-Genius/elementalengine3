///============================================================================
/// \file		CMathManager.h
/// \brief		Header file for the Math Manager
/// \date		08-12-2005
/// \author		Brian Bazyk
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
///============================================================================

#ifndef	MATHMANAGER_H
#define	MATHMANAGER_H

#include "CManager.h"

namespace ElementalEngine
{

class CMathManager : public CManager
{
protected:
	SINGLETONCONSTRUCTOROVERRIDE(CMathManager);
	/// Default Constructor
	CMathManager();
public:
	/// Default Deconstructor
	~CMathManager();

	/// Function to get this singleton component
	/// \return	IComponent*: This manager
	static IComponent *Create(int nArgs, va_list argptr);

	// ------------------ Messages ------------------ //
	/// Given an implementation name uid, returns a pointer to that implementation.
	DWORD OnGetActiveMathImplementation(DWORD size, void *in_params);
	/// Registers a math implementation
	DWORD OnRegisterMathImplementation(DWORD size, void *in_params);

private:
	/// Load and Initialize all DLMs
	void LoadDLMs();
	/// Load single DLM
	void LoadDLM(const TCHAR *pathName);
	/// Initialize all loaded DLMs
	void InitAllDLMS();
	/// Find all DLMs and load them
	void SearchDLMS(const TCHAR *pathName);

	/// Map of a math library (dlm) name to it's implementation map
	typedef map<DWORD, MATHIMPLEMENTATIONMAP> MATHLIBRARYMAP;
	/// Loaded math libraries
	MATHLIBRARYMAP m_MathLibraryMap;
	/// Currently loaded math library
	DWORD m_dwCurrentMathLibrary;
	int m_iCurrentMathLibraryPriority;

	/// map of dll priorities to handles
	DLLPRIORITYMAP m_DLLPriorityMap;
};

}
#endif