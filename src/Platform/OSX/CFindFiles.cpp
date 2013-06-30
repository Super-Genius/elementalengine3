
#include <stdafx.h>
#include "CFindFiles.h"

namespace ElementalEngine
{

IFindFiles *GetFindFiles()
{
	return SINGLETONINSTANCE(CFindFiles);
}


HANDLE CFindFiles::FindFirstFile(const TCHAR *filename, FIND_DATA *FindFileData)
{
	return 0;
}

BOOL CFindFiles::FindNextFile(HANDLE hFindFile, FIND_DATA *FindFileData)
{
	return false;
}

BOOL CFindFiles::FindClose(HANDLE hFindFile)
{
	return true;
}

}
