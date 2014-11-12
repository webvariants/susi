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

#ifndef __BASICAPICLIENT__
#define __BASICAPICLIENT__

#include "apiserver/JSONTCPClient.h"
#include "events/EventManager.h"
#include "util/Any.h"

namespace Susi {
    namespace Api {

        class BasicApiClient : protected JSONTCPClient {
        protected:
            virtual void onMessage( Susi::Util::Any & message ) override;

        public:
            BasicApiClient( std::string addr ) : JSONTCPClient {addr} {}

            virtual ~BasicApiClient() {
                close();
            }

            void close() {
                try{
                    sendShutdown();
                    JSONTCPClient::close();
                }catch(...){}
            }

            virtual void onConsumerEvent( Susi::Events::Event & event ) {};
            virtual void onProcessorEvent( Susi::Events::Event & event ) {};
            virtual void onAck( Susi::Events::Event & event ) {};

            void sendPublish( Susi::Events::Event & event );
            void sendRegisterConsumer( std::string topic , std::string name = "");
            void sendRegisterProcessor( std::string topic , std::string name = "");
            void sendAck( Susi::Events::Event & event );
            void sendUnregisterConsumer( std::string topic  );
            void sendUnregisterProcessor( std::string topic  );
            void sendShutdown();

        };

    }
}

#endif // __BASICAPICLIENT__
