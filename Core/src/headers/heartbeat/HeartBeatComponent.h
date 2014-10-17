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

#ifndef __HEARTBEATCOMPONENT__
#define __HEARTBEATCOMPONENT__

#include <thread>
#include <atomic>
#include "world/BaseComponent.h"

namespace Susi {
    class HeartBeatComponent : public System::BaseComponent {
    protected:
        std::atomic<bool> stopVar;
        std::thread t;
    public:
        HeartBeatComponent( System::ComponentManager * mgr ) :
            System::BaseComponent {mgr},
        stopVar {false} {}

        ~HeartBeatComponent() {
            stop();
            Susi::Logger::info( "stopped HeartBeatComponent" );
        }

        virtual void start() override {
            t = std::move( std::thread {
                [this]() {
                    int count = 0;
                    std::chrono::seconds interval( 1 );
                    while( !this->stopVar.load() ) {
                        ++count %= 300;
                        auto evt = createEvent("heartbeat::one");
                        evt->authlevel = 3;
                        publish( std::move(evt) );
                        publish( createEvent( "heartbeat::one" ) );
                        if( count % 5 == 0 ) {
                            publish( createEvent( "heartbeat::five" ) );
                        }
                        if( count % 10 == 0 ) {
                            publish( createEvent( "heartbeat::ten" ) );
                        }
                        if( count % 60 == 0 ) {
                            publish( createEvent( "heartbeat::minute" ) );
                        }
                        if( count % 300 == 0 ) {
                            publish( createEvent( "heartbeat::fiveMinute" ) );
                        }
                        std::this_thread::sleep_for( interval );
                    }
                }
            } );

            Susi::Logger::info( "started HeartBeatComponent" );
        }

        virtual void stop() override {
            stopVar.store( true );
            if( t.joinable() )t.join();
        }
    };
}

#endif // __HEARTBEATCOMPONENT__

