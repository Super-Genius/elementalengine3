///============================================================================
/// \file		IFileIOSupport.h
/// \brief		Definition of interface to file IO
/// \date		07-21-2009
/// \author		Kenneth Hurley
/// \author		Marivaldo Cabral
/// \note   Elemental Engine
///         Copyright (c)  2005-2008 Signature Devices, Inc.
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

#ifndef IFILEIOSUPPORT_H__
#define IFILEIOSUPPORT_H__

/// \class IFileIOSupport 
/// \brief Interface to manipulate file in all platforms
class IFileIOSupport 
{
public :
	virtual ~IFileIOSupport() {}
	virtual void Init( ) = 0;

	virtual void DeInit() = 0;

	virtual int	OpenFile(const char * name, __file_modes mode, __file_handle * handle) = 0;

	virtual int OpenTempFile(__file_handle * handle) = 0;

	virtual int ReadFile(__file_handle handle, unsigned char * buffer, size_t * count, __ref_con ref_con) = 0;

	virtual int WriteFile(__file_handle handle, unsigned char * buffer, size_t * count, __ref_con ref_con) = 0;

	virtual int PositionFile(__file_handle handle, fpos_t * position, int mode, __ref_con ref_con) = 0;

	virtual int CloseFile(__file_handle handle) = 0;

	virtual void TempFileName(char * name_str, void * filespec) = 0;

	virtual int DeleteFile(const char * name) = 0;

	virtual int RenameFile(const char * old_name, const char * new_name) = 0;

	virtual void SetAsyncFlag(void) { }

	virtual bool IsAsyncReadFinished(void) { return true; }

	virtual void SetAsyncReadNotifyCallback(IOCALLBACK callback) { }

	// Functions to handle directories
	virtual int CreateDirectory(const char * name) { return __no_io_error; }

	virtual int DeleteDirectory(const char * name) { return __no_io_error; }

	virtual const char * GetSaveDataDirectory(void) { return ""; }

	virtual int StatFile(const char * name, struct stat *statPtr) { return 0; }

	virtual int FStatFile(int handle, struct stat *statPtr) { return 0; }

	virtual DIR *OpenDirectory(const char *path) { return NULL; }

	virtual struct dirent *ReadDirectory(DIR *) { return NULL; }

	virtual void RewindDirectory(DIR *) { return; }

	virtual int CloseDirectory(DIR *) { return 0; }

	virtual void LockIOSupport( ) { }

	virtual void UnlockIOSupport( ) { }

};



#endif	// #ifndef IFILEIOSUPPORT_H__