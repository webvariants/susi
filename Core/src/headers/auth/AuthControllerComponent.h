/*
 * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 *
 * @author: Christian Sonderfeld (christian.sonderfeld@webvariants.de)
 */


#ifndef __AUTH_CONTROLLERCOMPONENT__
#define __AUTH_CONTROLLERCOMPONENT__

#include "auth/AuthController.h"
#include "world/BaseComponent.h"

namespace Susi {
    namespace Auth {

        class ControllerComponent : public Controller, public Susi::System::BaseComponent
        {
        protected:
            void handleLogin( Susi::Events::EventPtr event );
            void handleLogout( Susi::Events::EventPtr event );
            void handleIsLoggedIn( Susi::Events::EventPtr event );
            void handleGetUsername( Susi::Events::EventPtr event );

            void handleAddUser( Susi::Events::EventPtr event );
            void handleDelUser( Susi::Events::EventPtr event );
            void handleUpdateUser( Susi::Events::EventPtr event );

        public:
            ControllerComponent( Susi::System::ComponentManager * mgr, std::string db_identifier ) :
                Controller {
                mgr->getComponent<Susi::DB::DBComponent>( "dbmanager" ),
                mgr->getComponent<Susi::Sessions::SessionManagerComponent>( "sessionmanager" ),
                db_identifier
            },
            Susi::System::BaseComponent {mgr} {}

            virtual void start() override {
                subscribe( std::string{"auth::login"}, Susi::Events::Processor{[this]( Susi::Events::EventPtr evt ) {
                    handleLogin( std::move( evt ) );
                }}, 3 );
                subscribe( std::string{"auth::logout"}, Susi::Events::Processor{[this]( Susi::Events::EventPtr evt ) {
                    handleLogout( std::move( evt ) );
                }}, 3 );
                subscribe( std::string{"auth::isLoggedIn"}, Susi::Events::Processor{[this]( Susi::Events::EventPtr evt ) {
                    handleIsLoggedIn( std::move( evt ) );
                }}, 3 );
                subscribe( std::string{"auth::getUsername"}, Susi::Events::Processor{[this]( Susi::Events::EventPtr evt ) {
                    handleGetUsername( std::move( evt ) );
                }}, 3 );


                subscribe( std::string{"auth::addUser"}, Susi::Events::Processor{[this]( Susi::Events::EventPtr evt ) {
                    assertAuthlevel(*evt,0);
                    handleAddUser( std::move( evt ) );
                }}, 0 );
                subscribe( std::string{"auth::delUser"}, Susi::Events::Processor{[this]( Susi::Events::EventPtr evt ) {
                    assertAuthlevel(*evt,0);
                    handleDelUser( std::move( evt ) );
                }}, 0 );
                subscribe( std::string{"auth::updateUser"}, Susi::Events::Processor{[this]( Susi::Events::EventPtr evt ) {
                    assertAuthlevel(*evt,0);
                    handleUpdateUser( std::move( evt ) );
                }}, 0 );
                Susi::Logger::info( "started AuthControllerComponent" );
            }

            virtual void stop() override {
                unsubscribeAll();
            }

            ~ControllerComponent() {
                stop();
                Susi::Logger::info( "stopped AuthControllerComponent" );
            }
        };

    }
}

#endif // __AUTH_CONTROLLERCOMPONENT__
