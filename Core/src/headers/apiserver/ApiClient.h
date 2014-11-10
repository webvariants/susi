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

#ifndef __APICLIENT__
#define __APICLIENT__

#include "apiserver/BasicApiClient.h"
#include "events/EventManager.h"

namespace Susi {
    namespace Api {

        class ApiClient : protected BasicApiClient, protected Susi::Events::Manager {
        public:
            ApiClient( std::string addr ) : BasicApiClient {addr} {}
            void close() {
                BasicApiClient::close();
            }
            virtual ~ApiClient() {
                close();
            }
            void publish( Susi::Events::EventPtr event, Susi::Events::Consumer finishCallback = Susi::Events::Consumer {} );
            long subscribe( std::string topic, Susi::Events::Processor processor , std::string name = "");
            long subscribe( std::string topic, Susi::Events::Consumer consumer , std::string name = "");
            bool unsubscribe( long id ) {
                subscribes.erase(id);
                return Susi::Events::Manager::unsubscribe( id );
            }
            void ack( Susi::Events::EventPtr event ) {
                Susi::Events::Manager::ack( std::move( event ) );
            }
            Susi::Events::EventPtr createEvent( std::string topic ) {
                return Susi::Events::Manager::createEvent( topic );
            }
        protected:
            virtual void onConsumerEvent( Susi::Events::Event & event ) override;
            virtual void onProcessorEvent( Susi::Events::Event & event ) override;
            virtual void onAck( Susi::Events::Event & event ) override;

            virtual void onConnect() override {};
            virtual void onClose() override {
                reconnect();
                for(auto & kv : subscribes) {
                    auto & sub = kv.second;
                    if(sub.processor){
                        sendRegisterProcessor(sub.topic, sub.name);
                    }else{
                        sendRegisterConsumer(sub.topic, sub.name);
                    }
                }
                for(auto & kv : publishData) {
                    sendPublish(*(kv.second.event));
                }
            };

            //remember for reconnect
            struct PublishData {
                Susi::Events::EventPtr event;
                Susi::Events::Consumer finishCallback;
            };
            std::map<std::string,PublishData> publishData;
            struct SubscribeData {
                std::string topic;
                std::string name;
                bool processor;
            };
            std::map<long,SubscribeData> subscribes;
        };

    }
}

#endif // __APICLIENT__
