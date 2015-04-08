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

#ifndef __DBCOMPONENT__
#define __DBCOMPONENT__

#include "susi/world/BaseComponent.h"
#include "susi/db/Manager.h"

namespace Susi {
    namespace DB {
        class DBComponent : public Susi::System::BaseComponent , public Manager {
        public:
            DBComponent( Susi::System::ComponentManager * mgr, BSON::Value & config ) :
                Susi::System::BaseComponent {mgr}, Manager {config} {}

            virtual void start() override {
                subscribe( std::string{"db::query"},Susi::Events::Processor{[this]( ::Susi::Events::EventPtr evt ) {
                    handleQuery( std::move( evt ) );
                }} );
                std::string msg = "started DatabaseComponent with "+std::to_string(Manager::dbMap.size())+" databases";
                LOG(INFO) <<  msg ;
            }

            virtual void stop() override {
                unsubscribeAll();
            }
            ~DBComponent() {
                stop();
                LOG(INFO) <<  "stopped DatabaseComponent" ;
            }
        protected:
            void handleQuery( Susi::Events::EventPtr  event );
        };
    }
}

#endif // __DBCOMPONENT__
