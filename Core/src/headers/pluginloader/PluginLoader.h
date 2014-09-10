#ifndef __PLUGINLOADER__
#define __PLUGINLOADER__

#include <Poco/ClassLoader.h>
#include <Poco/Manifest.h>
#include <Poco/DirectoryIterator.h>
#include <iostream> 
#include "pluginloader/Plugin.h"

namespace Susi {

class PluginLoader : Poco::ClassLoader<Susi::Plugin> {
public:
	PluginLoader(std::string plugin);
};

}

#endif // __PLUGINLOADER__