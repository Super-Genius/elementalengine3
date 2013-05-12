


#include <stdafx.h>
#ifndef _LIB
#include "DLLVersion.h"
#endif

#include "CPluginLoader.h"

namespace ElementalEngine
{

IPluginLoader *GetPluginLoader()
{
	return SINGLETONINSTANCE(CPluginLoader);
}

int CPluginLoader::LoadPlugin(TCHAR *pathName, HINSTANCE *outInstance, DWORD *priority)
{
	if ((pathName == NULL) || (outInstance == NULL) || (priority == NULL))
	{
		return ERROR_INVALID_PARAMETER;
	}

#ifndef _LIB

	GETDLLVERSION GetDLLVersion;
	GETPRIORITY GetPriority;
	DWORD version;
	TCHAR Mess[2048];
	HINSTANCE dllHandle;

	// try loading the dll
	dllHandle = LoadLibrary(pathName);
	if (dllHandle == NULL)
	{
		LPVOID lpMsgBuf = _T("");
		DWORD lastErr = GetLastError();

		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | 
				FORMAT_MESSAGE_IGNORE_INSERTS, NULL, lastErr, 
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
				(LPTSTR) &lpMsgBuf, 0, NULL);
		_stprintf( Mess, _T("Error Loading DLL:%s\n%s\n"), pathName, lpMsgBuf);
		EngineGetToolBox()->Log( LOGERROR, Mess );
		return lastErr;
	}

	// get version function
	GetDLLVersion = (GETDLLVERSION)GetProcAddress(dllHandle, _T("GetDLLVersion"));
	if (GetDLLVersion == NULL)
	{
		_stprintf( Mess, _T("Unable to load %s\n"
			"Unable to locate version function.\n"), pathName);
		EngineGetToolBox()->Log( LOGERROR, Mess );
		FreeLibrary(dllHandle);
		*outInstance = NULL;
		return ERROR_INVALID_FUNCTION;
	}

	// get version
	version = (*GetDLLVersion)() & ~1;
	// check version
	if (version != (INTERFACEDLLVERSION & ~1))
	{
		_stprintf( Mess, _T("Unable to load %s\n"
			"version %x doesn't match expected %x.\n"), pathName, version, INTERFACEDLLVERSION);
		EngineGetToolBox()->Log( LOGERROR, Mess );
		FreeLibrary(dllHandle);
		*outInstance = NULL;
		return ERROR_INVALID_FUNCTION;
	}

	// get the priority function
	GetPriority = (GETPRIORITY)GetProcAddress(dllHandle, _T("GetPriority"));
	if (GetPriority == NULL)
	{
		_stprintf( Mess, _T("Unable to load %s\n"
				"GetPriority routine failed.\n"), pathName);
		EngineGetToolBox()->Log( LOGERROR, Mess );
		FreeLibrary(dllHandle);
		*outInstance = NULL;
		return ERROR_INVALID_FUNCTION;
	}

	*outInstance = dllHandle;
	*priority = (*GetPriority)();

#endif

	return ERROR_SUCCESS;
}

int CPluginLoader::UnloadPlugin(HINSTANCE instance)
{
	TCHAR Mess[2048];

	// get the DeInit function
	DEINITDLL DeinitDLL = (DEINITDLL)GetProcAddress(instance, _T("DeinitDLL"));
	if (DeinitDLL == NULL)
	{
		_stprintf( Mess, _T("Unable to deinit DLL for handle %d\n"
			"DEinitDLL routine failed.\n"), instance);
		EngineGetToolBox()->Log( LOGERROR, Mess );
		return ERROR_INVALID_FUNCTION;	
	}

	(*DeinitDLL)();

	FreeLibrary(instance);

	return ERROR_SUCCESS;

}

int CPluginLoader::InitPlugin(HINSTANCE instance)
{
	TCHAR Mess[2048];

	// get the init function
	INITDLL initDLL = (INITDLL)GetProcAddress(instance, _T("InitDLL"));
	if (initDLL == NULL)
	{
		_stprintf( Mess, _T("Unable to init DLL for handle %d\n"
			"InitDLL routine failed.\n"), instance);
		EngineGetToolBox()->Log( LOGERROR, Mess );
		return ERROR_INVALID_FUNCTION;	
	}

	(*initDLL)();

	return ERROR_SUCCESS;
}

}

