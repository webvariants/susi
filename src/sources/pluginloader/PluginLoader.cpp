#include "pluginloader/PluginLoader.h"

Susi::PluginLoader::PluginLoader(std::string location){
	Poco::DirectoryIterator end;
	for (Poco::DirectoryIterator it(location); it != end; ++it){
		Poco::Path path = it->path();
		std::string ext = path.getExtension();
		if ("."+ext == Poco::SharedLibrary::suffix()) {
			try{
				loadLibrary(path.toString(),"");
				for(auto manifest : *this){
					std::cout<<"Loaded library: "<<manifest->first<<std::endl;
					Poco::Manifest<Susi::Plugin>::Iterator itManifest(manifest->second->begin());
					Poco::Manifest<Susi::Plugin>::Iterator endManifest(manifest->second->end());
					for ( ; itManifest != endManifest; ++itManifest){
						std::cout<<"Loaded class: "<<itManifest->name()<<std::endl;
						Susi::Plugin *pPlugin = itManifest->create();
						itManifest->autoDelete(pPlugin);
						pPlugin->run();
					}
				}
			}catch(const std::exception & e){
				std::cerr<<e.what()<<" "<<path.toString()<<std::endl;
			}
		}
	}
}