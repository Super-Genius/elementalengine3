
#ifndef _PLUGIN_LOADER_H_
#define _PLUGIN_LOADER_H_

#include "IPluginLoader.h"

namespace ElementalEngine
{

class CPluginLoader : public IPluginLoader
{
private:
	SINGLETON( CPluginLoader );

public:
	virtual int LoadPlugin(TCHAR *pathName, HINSTANCE *outInstance, DWORD *priority);
	virtual int UnloadPlugin(HINSTANCE instance);
	virtual int InitPlugin(HINSTANCE instance);
};

}
#endif	// #ifndef _PLUGIN_LOADER_H_