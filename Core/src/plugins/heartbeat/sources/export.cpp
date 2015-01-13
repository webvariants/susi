/*
 * Copyright (c) 2015, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 *
 * @author: Tino Rusch (tino.rusch@webvariants.de)
 */

#include <thread>
#include <atomic>
#include "world/BaseComponent.h"

#include "logger/easylogging++.h"

SHARE_EASYLOGGINGPP(el::Helpers::storage());

#if defined(_WIN32)
#define LIBRARY_API __declspec(dllexport)
#else
#define LIBRARY_API
#endif

extern "C" {
    std::shared_ptr<Susi::System::Component> LIBRARY_API createComponent(Susi::System::ComponentManager *mgr, Susi::Util::Any &config);
    std::string LIBRARY_API getName();
    std::vector<std::string> LIBRARY_API getDependencies();
}

namespace Susi {
    class HeartBeatComponent : public System::BaseComponent {
    protected:
        std::atomic<bool> stopVar;
        std::thread t;
    public:
        HeartBeatComponent( System::ComponentManager * mgr , Susi::Util::Any & config) :
            System::BaseComponent {mgr},
        stopVar {false} {

            LOG(INFO) << "constructed heartbeat.";
        }

        ~HeartBeatComponent() {
            stop();
            LOG(INFO) << "stopped HeartBeatComponent" ;
        }

        virtual void start() override {
            LOG(ERROR) << "starting heartbeat!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
            LOG(DEBUG) << "starting heartbeat!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
            t = std::move( std::thread {
                [this]() {
                    int count = 0;
                    std::chrono::milliseconds interval( 100 );
                    while( !this->stopVar.load() ) {
                        ++count %= 3000;
                        if( count % 10 == 0) {
                            publish( createEvent( "heartbeat::one" ) );
                        }
                        if( count % 50 == 0 ) {
                            publish( createEvent( "heartbeat::five" ) );
                        }
                        if( count % 100 == 0 ) {
                            publish( createEvent( "heartbeat::ten" ) );
                        }
                        if( count % 600 == 0 ) {
                            publish( createEvent( "heartbeat::minute" ) );
                        }
                        if( count % 3000 == 0 ) {
                            publish( createEvent( "heartbeat::fiveMinute" ) );
                        }
                        std::this_thread::sleep_for( interval );
                    }
                }
            } );

        }

        virtual void stop() override {
            stopVar.store( true );
            if( t.joinable() )t.join();
        }
    };
}

std::shared_ptr<Susi::System::Component> LIBRARY_API createComponent(Susi::System::ComponentManager * mgr, Susi::Util::Any & config){
    return std::make_shared<Susi::HeartBeatComponent>(mgr,config);
}
std::string LIBRARY_API getName(){
    return "heartbeat";
}

std::vector<std::string> LIBRARY_API getDependencies(){
    return {"logger","eventsystem"};
}