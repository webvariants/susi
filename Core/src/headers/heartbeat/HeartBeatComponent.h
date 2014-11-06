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
            LOG(INFO) <<  "stopped HeartBeatComponent" ;
        }

        virtual void start() override {
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

            LOG(INFO) <<  "started HeartBeatComponent" ;
        }

        virtual void stop() override {
            stopVar.store( true );
            if( t.joinable() )t.join();
        }
    };
}

#endif // __HEARTBEATCOMPONENT__

