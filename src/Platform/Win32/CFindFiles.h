
///============================================================================
/// \file	CFindFiles.h
/// \note   Elemental Engine
/// \author	Kenneth Hurley
///         Copyright (c)  2005-2013 Signature Devices, Inc.
///
///         This code is redistributable under the terms of the EE License.
///
///         This code is distributed without warranty or implied warranty of
///			merchantability or fitness for a particular purpose. See the 
///			EE License for more details.
///
///         You should have received a copy of the EE License along with this
///			code; If not, write to Signature Devices, Inc.,
///			3200 Bridge Parkway Suite 102, Redwood City, CA 94086 USA.
///============================================================================ 

#ifndef _CFIND_FILES_H_
#define _CFIND_FILES_H_

#include "IFindFiles.h"

namespace ElementalEngine
{

class CFindFiles : public IFindFiles
{
private:
	SINGLETON( CFindFiles );

public:

	virtual HANDLE FindFirstFile(const TCHAR *filename, FIND_DATA *FindFileData);

	virtual BOOL FindNextFile(HANDLE hFindFile, FIND_DATA *FindFileData);

	virtual BOOL FindClose(HANDLE hFindFile);
};

}

#endif	// #ifndef _CFIND_FILES_H_

