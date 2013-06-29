///============================================================================
/// \file	IFindFiles.h
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
///			303 Twin Dolphin Drive Suite 600, Redwood City, CA 94086 USA.
///============================================================================ 

#ifndef _IFIND_FILES_H_
#define _IFIND_FILES_H_

namespace ElementalEngine
{

class IFindFiles
{

public:
	/// Default destructor so RTTI and deletes will happen
	virtual ~IFindFiles() {};

	/// \brief	find the first file
	/// \param	filename = explicit path and name or path + wildcards
	/// \param  FindFileData = structure of find file data that is platform specific
	/// \return	a handle to be used in subsequent FindNextFile for FindClose
	virtual HANDLE FindFirstFile(const TCHAR *filename, FIND_DATA *FindFileData) = 0;

	/// \brief	find the next file
	/// \param	input handle from FindFirstFile
	/// \param  FindFileData = structure of find file data that is platform specific
	/// \return boolean false = non success
	virtual BOOL FindNextFile(HANDLE hFindFile, FIND_DATA *FindFileData) = 0;

	/// \brief close the find file handle
	/// \param	input handle from FindFirstFile
	/// \return boolean false = non success
	virtual BOOL FindClose(HANDLE hFindFile) = 0;
};

}

#endif	// #ifndef _IFIND_FILES_H_

