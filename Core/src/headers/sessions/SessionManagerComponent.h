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

#ifndef __SESSION_MANAGER_COMPONENT__
#define __SESSION_MANAGER_COMPONENT__

#include "world/BaseComponent.h"
#include "sessions/SessionManager.h"

namespace Susi {
    namespace Sessions {
        class SessionManagerComponent : public Susi::System::BaseComponent , public SessionManager {
        public:
            SessionManagerComponent( Susi::System::ComponentManager * mgr, std::chrono::milliseconds stdSessionLifetime ) :
                Susi::System::BaseComponent {mgr}, SessionManager {} {
                _stdSessionLifetime = stdSessionLifetime;
            }

            ~SessionManagerComponent() {
                stop();
                Susi::Logger::info( "stopped SessionManagerComponent" );
            }

            virtual void start() override {
                init( _stdSessionLifetime );

                // Consumer
                Susi::Events::Consumer handler = [this]( ::Susi::Events::SharedEventPtr evt ) {
                    handleCheckSessions( std::move( evt ) );
                };
                subscribe( "heartbeat::one", handler );

                // Processor
                subscribe( "session::setAttribute", [this]( ::Susi::Events::EventPtr evt ) {
                    handleSetAttribute( std::move( evt ) );
                } );
                subscribe( "session::getAttribute", [this]( ::Susi::Events::EventPtr evt ) {
                    handleGetAttribute( std::move( evt ) );
                } );
                subscribe( "session::pushAttribute", [this]( ::Susi::Events::EventPtr evt ) {
                    handlePushAttribute( std::move( evt ) );
                } );
                subscribe( "session::removeAttribute", [this]( ::Susi::Events::EventPtr evt ) {
                    handleRemoveAttribute( std::move( evt ) );
                } );
                subscribe( "session::update", [this]( ::Susi::Events::EventPtr evt ) {
                    handleUpdate( std::move( evt ) );
                } );
                subscribe( "session::check", [this]( ::Susi::Events::EventPtr evt ) {
                    handleCheck( std::move( evt ) );
                } );
                Susi::Logger::info( "started SessionManagerComponent" );
            }

            virtual void stop() override {
                unsubscribeAll();
            }

        protected:
            std::chrono::milliseconds _stdSessionLifetime;

            void handleCheckSessions( Susi::Events::SharedEventPtr event );
            void handleGetAttribute( Susi::Events::EventPtr event );
            void handleSetAttribute( Susi::Events::EventPtr event );
            void handlePushAttribute( Susi::Events::EventPtr event );
            void handleRemoveAttribute( Susi::Events::EventPtr event );
            void handleUpdate( Susi::Events::EventPtr event );
            void handleCheck( Susi::Events::EventPtr event );
        };
    }
}

#endif // __SESSION_MANAGER_COMPONENT__
