
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
	return ::FindFirstFile(filename, FindFileData);
}

BOOL CFindFiles::FindNextFile(HANDLE hFindFile, FIND_DATA *FindFileData)
{
	return ::FindNextFile(hFindFile, FindFileData);
}

BOOL CFindFiles::FindClose(HANDLE hFindFile)
{
	return ::FindClose(hFindFile);
}

}
