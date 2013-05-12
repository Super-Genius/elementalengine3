

#ifndef _IPLUGIN_LOADER_H_
#define _IPLUGIN_LOADER_H_

namespace ElementalEngine
{

class IPluginLoader
{
public:
	virtual ~IPluginLoader() {};

	/// Load plugin, loads the plugin into memory
	/// \param pathName = full path + name of plugin to load
	/// \param outInstance = a pointer to handle to the instance of the plugin loaded
	/// \param priority = a pointer to a DWORD to save priority to after retrieved
	/// \returns - Error or 0 for OK
	virtual int LoadPlugin(TCHAR *pathName, HINSTANCE *outInstance, DWORD *priority) = 0;

	/// Unload plugin, unloads the plugin by freeing and DeIniting it
	/// \param instance = handle to the instance of the plugin loaded
	/// \returns - Error or 0 for OK
	virtual int UnloadPlugin(HINSTANCE instance) = 0;

	/// Initialize the plugin
	/// \param instance = handle to the instance of the plugin loaded
	/// \returns - Error or 0 for OK
	virtual int InitPlugin(HINSTANCE instance) = 0;
};

}
#endif	//#ifndef _IPLUGIN_LOADER_H_
