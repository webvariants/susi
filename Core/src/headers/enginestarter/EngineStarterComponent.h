/*
 * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 *
 * @author: Thomas Krause (thomas.krause@webvariants.de)
 */

#ifndef __ENGINE_STARTER_COMPONENT__
#define __ENGINE_STARTER_COMPONENT__

#include "world/BaseComponent.h"
#include "enginestarter/EngineStarter.h"

namespace Susi {
    namespace EngineStarter {
        class StarterComponent : public Susi::System::BaseComponent , public Starter {
        public:
            StarterComponent( Susi::System::ComponentManager * mgr ) :
                Susi::System::BaseComponent {mgr}, Starter {} {}

            virtual void start() override {
                subscribe( "enginestarter::start", [this]( Susi::Events::EventPtr evt ) {
                    handleStart( std::move( evt ) );
                } );
                subscribe( "enginestarter::restart", [this]( Susi::Events::EventPtr evt ) {
                    handleRestart( std::move( evt ) );
                } );
                subscribe( "enginestarter::stop", [this]( Susi::Events::EventPtr evt ) {
                    handleStop( std::move( evt ) );
                } );

                subscribe( "global::start", [this]( Susi::Events::EventPtr evt ) {
                    handleStart( std::move( evt ) );
                } );
                subscribe( "global::restart", [this]( Susi::Events::EventPtr evt ) {
                    handleRestart( std::move( evt ) );
                } );
                subscribe( "global::stop", [this]( Susi::Events::EventPtr evt ) {
                    handleStop( std::move( evt ) );
                } );
                LOG(INFO) <<  "started EngineStarterComponent" ;
            }

            virtual void stop() override {
                unsubscribeAll();
                killall();
            }

            ~StarterComponent() {
                stop();
                LOG(INFO) <<  "stopped EngineStarterComponent" ;
            }
        protected:
            void handleStart( Susi::Events::EventPtr event );
            void handleRestart( Susi::Events::EventPtr event );
            void handleStop( Susi::Events::EventPtr event );
        };
    }

}

#endif // __ENGINE_STARTER_COMPONENT__
