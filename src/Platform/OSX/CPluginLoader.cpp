


#include <stdafx.h>
#ifndef _LIB
#include "DLLVersion.h"
#endif

#include "CPluginLoader.h"

#include <mach-o/dyld.h>
#include <dlfcn.h>

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

	GETDLLVERSION GetDLLVersion;
	GETPRIORITY GetPriority;
	DWORD version;
	TCHAR Mess[2048];
	void *dllHandle;
    
    // should be converted to TCHAR
    TCHAR *exePath = NULL;
    uint32_t bufSize = 0;
    
    _NSGetExecutablePath(exePath, &bufSize);

    exePath = (char *)malloc((bufSize + _tcsclen(pathName) + 2) * sizeof(TCHAR));
    
    if (_NSGetExecutablePath(exePath, &bufSize) != 0)
    {
        _stprintf( Mess, _T("Error Loading DLL:%s\nCannot get executable path\n"), pathName);
		EngineGetToolBox()->Log( LOGERROR, Mess );
    }

    _tcscat(exePath, "/");
    // conversion of buffer if TCHAR = wchar_t type should be done here.
    _tcscat(exePath, pathName);
    
	// try loading the dll
	dllHandle = dlopen(exePath, RTLD_NOW);
	if (dllHandle == NULL)
	{
		_stprintf( Mess, _T("Error Loading DLL:%s\n%s\n"), pathName, dlerror());
		EngineGetToolBox()->Log( LOGERROR, Mess );
		return ERROR_INVALID_FUNCTION;
	}

	// get version function
	GetDLLVersion = (GETDLLVERSION)dlsym(dllHandle, _T("GetDLLVersion"));
	if (GetDLLVersion == NULL)
	{
		_stprintf( Mess, _T("Unable to load %s\n"
			"Unable to locate version function.\n"), pathName);
		EngineGetToolBox()->Log( LOGERROR, Mess );
		dlclose(dllHandle);
		*outInstance = NULL;
		return ERROR_INVALID_FUNCTION;
	}

	// get version
	version = (*GetDLLVersion)() & ~1;
	// check version
	if (version != (INTERFACEDLLVERSION & ~1))
	{
		_stprintf( Mess, _T("Unable to load %s\n"
			"version %x doesn't match expected %x.\n"), pathName, (unsigned int)version, INTERFACEDLLVERSION);
		EngineGetToolBox()->Log( LOGERROR, Mess );
		dlclose(dllHandle);
		*outInstance = NULL;
		return ERROR_INVALID_FUNCTION;
	}

	// get the priority function
	GetPriority = (GETPRIORITY)dlsym(dllHandle, _T("GetPriority"));
	if (GetPriority == NULL)
	{
		_stprintf( Mess, _T("Unable to load %s\n"
				"GetPriority routine failed.\n"), pathName);
		EngineGetToolBox()->Log( LOGERROR, Mess );
		dlclose(dllHandle);
		*outInstance = NULL;
		return ERROR_INVALID_FUNCTION;
	}

	*outInstance = dllHandle;
	*priority = (*GetPriority)();

	return ERROR_SUCCESS;
}

int CPluginLoader::UnloadPlugin(HINSTANCE instance)
{
	TCHAR Mess[2048];

	// get the DeInit function
	DEINITDLL DeinitDLL = (DEINITDLL)dlsym(instance, _T("DeinitDLL"));
	if (DeinitDLL == NULL)
	{
		_stprintf( Mess, _T("Unable to deinit DLL for handle %p\n"
			"DeinitDLL routine failed.\n"), instance);
		EngineGetToolBox()->Log( LOGERROR, Mess );
		return ERROR_INVALID_FUNCTION;
	}

	(*DeinitDLL)();

	dlclose(instance);

	return ERROR_SUCCESS;

}

int CPluginLoader::InitPlugin(HINSTANCE instance)
{

	TCHAR Mess[2048];

	// get the init function
	INITDLL initDLL = (INITDLL)dlsym(instance, _T("InitDLL"));
	if (initDLL == NULL)
	{
		_stprintf( Mess, _T("Unable to init DLL for handle %p\n"
			"InitDLL routine failed.\n"), instance);
		EngineGetToolBox()->Log( LOGERROR, Mess );
		return ERROR_INVALID_FUNCTION;	
	}

	(*initDLL)();
    
	return ERROR_SUCCESS;
}

}

