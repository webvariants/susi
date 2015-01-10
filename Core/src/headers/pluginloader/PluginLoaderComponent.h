/*
 * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 *
 * @author: Tino Rusch (tino.rusch@webvariants.de)
 */

#ifndef __PLUGINLOADERCOMPONENT__
#define __PLUGINLOADERCOMPONENT__

#include "world/BaseComponent.h"

#include <Poco/Glob.h>
#include <Poco/SharedLibrary.h>

typedef std::shared_ptr<Susi::System::Component>(*CreateComponent_t)(Susi::System::ComponentManager*); 

namespace Susi {
    class PluginLoaderComponent : public System::BaseComponent {
    protected:
        std::set<std::string> sharedLibraryFiles;
        std::vector<std::shared_ptr<Susi::System::Component>> plugins;

        const std::string symbol_createComponent = "createComponent";

    public:
        PluginLoaderComponent(System::ComponentManager * mgr ) :
            System::BaseComponent {mgr} {}

        ~PluginLoaderComponent() {
            stop();
        }

        virtual void start() override {
            std::string pattern = "**/*";
            pattern += Poco::SharedLibrary::suffix();
            LOG(INFO) << "lib pattern: "<<pattern;
            Poco::Glob::glob(pattern, sharedLibraryFiles);

            for(auto & path : sharedLibraryFiles){
                LOG(INFO) << "found lib " << path;
                Poco::SharedLibrary lib{path};
                if(lib.hasSymbol(symbol_createComponent)){
                    CreateComponent_t createComponent = (CreateComponent_t)lib.getSymbol(symbol_createComponent);
                    auto component = createComponent(componentManager);
                    component->start();
                    plugins.push_back(component);
                }
            }

            LOG(INFO) <<  "started PluginLoaderComponent" ;
        }

        virtual void stop() override {
            LOG(INFO) <<  "stopped PluginLoaderComponent" ;
        }
    };
}

#endif // __PLUGINLOADERCOMPONENT__

