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

#ifndef __REMOTEEVENTCOLLECTOR__
#define __REMOTEEVENTCOLLECTOR__

#include "apiserver/BasicApiClient.h"
#include "events/IEventSystem.h"

namespace Susi {
    namespace Autodiscovery {

        class RemoteEventCollector : protected Susi::Api::BasicApiClient {
        protected:
            std::string _ownId;
            std::shared_ptr<Susi::Events::IEventSystem> _eventsystem;

            virtual void onProcessorEvent( Susi::Events::Event & event ) {
                auto eventPtr = _eventsystem->createEvent( event.topic );
                *eventPtr = event;
                eventPtr->topic = eventPtr->topic.substr( 0,eventPtr->topic.find( "@" ) );
                Susi::Events::Consumer finishCallback = [this]( Susi::Events::SharedEventPtr evt ) {
                    sendAck( *evt );
                };
                _eventsystem->publish( std::move( eventPtr ), finishCallback );
            }

        public:

            RemoteEventCollector(
                std::string addr,
                std::string ownId,
                std::shared_ptr<Susi::Events::IEventSystem> eventsystem ) :
                Susi::Api::BasicApiClient {addr}, _ownId {ownId}, _eventsystem {eventsystem}
            {
                if( _eventsystem.get() == nullptr ) {
                    throw std::runtime_error {"no event manager supplied"};
                }
                sendRegisterProcessor( "*@"+_ownId );
            }

        };

    }
}

#endif // __REMOTEEVENTCOLLECTOR__
