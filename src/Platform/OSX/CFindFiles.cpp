
#include <stdafx.h>
#include "CFindFiles.h"
#include <dirent.h>
#include <sys/stat.h>

#define min(a,b) ((a <= b) ? a : b)
namespace ElementalEngine
{

IFindFiles *GetFindFiles()
{
	return SINGLETONINSTANCE(CFindFiles);
}

const char SingleCharPatt =		_T('?');	// single character wild card matches any character except a NULL.
const char MultiCharPatt =		_T('*');	// matches any number of characters.  e.g., '*bc' matches 'f3+sd2sdfbc'
const char NegativeMatchPatt =	_T('-');	// negates a match character.  '-abc' matches 3 chars not begining with 'a'
const char OpenGroupPatt =		_T('[');	// groups characters into an 'or' group.  E.g., [0123456789] will match a digit.
const char CloseGroupPatt =		_T(']');	// you must have one of these to close the OpenGroupPatt.
    
/// return true if the pattern matches the string
/// param: string compare this string against this
/// param: Pat pattern to match.  Pattern chars are documented above.
BOOL match(char *string, char *Pat )
{
    for(;;) {
        // Match any single character ?
        if( *Pat == SingleCharPatt ) {
            if( *string++ == 0 )
                return FALSE;
            Pat++;
        }
        
        // match any length of chararcters
        if( *Pat == MultiCharPatt ) {	// match 0 or more chars
            Pat++;
            for( ;; ){
                if( match( string, Pat ) )
                    return TRUE;
                else if( *string++ == 0 )
                    return FALSE;
            }
        }
        // match anything but ...
        if( *Pat == NegativeMatchPatt ) {
            Pat++;
            return( !match( string, Pat ) );
        }
        
        // match any one of these...
        if( *Pat == OpenGroupPatt ) {
            for( ;; ) {
                Pat++;
                if( *Pat == CloseGroupPatt )
                    return FALSE;
                else if( *Pat == *string ) {
                    while( *Pat++ != CloseGroupPatt )
                        ;
                    string++;
                    break;
                }
            }
            continue;
        }
        // if not done, then do any chars match?
        if( *Pat == 0 && *string == 0 )
            return TRUE;
        else if( toupper(*Pat++) != toupper(*string++) )
            return FALSE ;
    }
}

char tmpSearch[PATH_MAX], wildCardSearch[PATH_MAX];
BOOL hasWildCard = FALSE;

HANDLE CFindFiles::FindFirstFile(const TCHAR *filename, FIND_DATA *FindFileData)
{
        
	strcpy(tmpSearch, (char *)filename);
	// See if the search string has a wildcard
	char *ptr = tmpSearch + strlen(tmpSearch);
	hasWildCard = FALSE;
    
	do
	{
		ptr--;
		hasWildCard = hasWildCard || (*ptr == '*');
	}
	while ((ptr != tmpSearch) && (*ptr != '/'));
    
	// If there's a wildcard, save the wildcard then remove it from the
	// original search. What will be left is the directory name.
	if (hasWildCard)
	{
		if (*ptr == '/')
		{
			ptr++;
			strcpy(wildCardSearch, ptr);
			*ptr = 0;
		}
		else
		{
			strcpy(wildCardSearch, tmpSearch);
			strcpy(tmpSearch, "/");
		}
	}
    
	DIR *handle = opendir(tmpSearch);
    
    if (handle != NULL)
    {
        if (!FindNextFile(handle, FindFileData))
        {
            return handle;
        }
        
        closedir(handle);
    }
    
	return NULL;
}

BOOL CFindFiles::FindNextFile(HANDLE hFindFile, FIND_DATA *FindFileData)
{
	BOOL ok = TRUE;
    //struct dirent *findData;
	
	while (ok)
	{
		struct dirent *DirEntry = readdir((DIR *)hFindFile);
		if (DirEntry == NULL)
		{
			ok = false;
		}
		else if ((!hasWildCard) || match(DirEntry->d_name, wildCardSearch))
		{
			//STATSTRUCT Stat;
            unsigned int maxCopy = min(strlen(DirEntry->d_name) + 1, PATH_MAX);
			strncpy((char *)FindFileData->cFileName, DirEntry->d_name, maxCopy);
            return FALSE;
		}
	}
    
	return TRUE;
}

BOOL CFindFiles::FindClose(HANDLE hFindFile)
{
    closedir((DIR *)hFindFile);
	return TRUE;
}

}
