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

#ifndef __AUTODISCOVERYCOMPONENT__
#define __AUTODISCOVERYCOMPONENT__

#include "autodiscovery/AutoDiscoveryManager.h"
#include "autodiscovery/RemoteEventCollector.h"
#include "world/BaseComponent.h"

namespace Susi {
    namespace Autodiscovery {

        class AutoDiscoveryComponent : public Susi::System::BaseComponent, public Susi::Autodiscovery::Manager {
        protected:

            class Collector : public Susi::Autodiscovery::RemoteEventCollector {
            protected:
                AutoDiscoveryComponent* _autodiscoveryComponent;
            public:
                Collector( AutoDiscoveryComponent *autodiscoveryComponent,
                           std::string addr,
                           std::string ownId,
                           std::shared_ptr<Susi::Events::IEventSystem> eventsystem ) :
                    Susi::Autodiscovery::RemoteEventCollector {addr,ownId,eventsystem},
                _autodiscoveryComponent {autodiscoveryComponent} {}

                virtual void onClose() override {
                    _autodiscoveryComponent->removeCollector( this );
                }
            };

            std::vector<std::shared_ptr<Collector>> _collectors;
            std::string _ownAddr;

            virtual void onNewHost( std::string & addr ) override {
                _collectors.emplace_back( new Collector {this,addr,_ownAddr,eventManager} );
            }

            void removeCollector( Collector* collector ) {
                for( size_t i=0; i<_collectors.size(); ++i ) {
                    if( _collectors[i].get() == collector ) {
                        _collectors.erase( _collectors.begin()+i );
                        break;
                    }
                }
            }

        public:

            AutoDiscoveryComponent( std::string mcastGroup, std::string ownAddr, Susi::System::ComponentManager* componentManager ) :
                Susi::System::BaseComponent {componentManager}, Susi::Autodiscovery::Manager {mcastGroup,ownAddr}, _ownAddr {ownAddr} {}

            virtual void start() override {
                Susi::Autodiscovery::Manager::start();
                LOG(INFO) <<  "started AutodiscoveryComponent" ;
            }

            virtual void stop() override {
                Susi::Autodiscovery::Manager::stop();
                while( _collectors.size() )_collectors.pop_back();
            }

            ~AutoDiscoveryComponent(){
                stop();
                LOG(INFO) <<  "stopped AutodiscoveryComponent" ;
            }

        };

    }
}

#endif // __AUTODISCOVERYCOMPONENT__
