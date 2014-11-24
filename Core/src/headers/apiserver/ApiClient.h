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

#include "world/Component.h"
#include "apiserver/BasicApiClient.h"
#include "events/EventManager.h"
#include "logger/easylogging++.h"

namespace Susi {
    namespace Api {

        class ApiClient : protected BasicApiClient, public Susi::Events::Manager {
        public:
            ApiClient( std::string addr ) : BasicApiClient {addr} {
                setMaxReconnectCount(5);
            }
            void close() {
                BasicApiClient::close();
            }
            virtual ~ApiClient() {
                close();
                LOG(INFO) << "Stopped ApiClient-Component.";
            }
            virtual void publish( Susi::Events::EventPtr event, Susi::Events::Consumer finishCallback = Susi::Events::Consumer {}, bool processorsOnly = false, bool consumersOnly = false );
            virtual long subscribe( std::string topic, Susi::Events::Processor processor , std::string name = "");
            virtual long subscribe( std::string topic, Susi::Events::Consumer consumer , std::string name = "");
            virtual bool unsubscribe( long id ) {
                return Susi::Events::Manager::unsubscribe( id );
            }
            virtual void ack( Susi::Events::EventPtr event ) {
                Susi::Events::Manager::ack( std::move( event ) );
            }
            virtual Susi::Events::EventPtr createEvent( std::string topic ) {
                return Susi::Events::Manager::createEvent( topic );
            }
            
        protected:
            virtual void onConsumerEvent( Susi::Events::Event & event ) override;
            virtual void onProcessorEvent( Susi::Events::Event & event ) override;
            virtual void onAck( Susi::Events::Event & event ) override;

            virtual void onConnect() override {};
            virtual void onClose() override {};
            virtual void onReconnect() override;

            struct PublishData {
                Susi::Events::Event event;
                Susi::Events::Consumer finishCallback;
            };
            std::map<std::string,PublishData> publishs;
            
            struct SubscribeData {
                std::string topic;
                std::string name;
                bool processor;
            };
            std::vector<SubscribeData> subscribes;

        };

        class ApiClientComponent : public ApiClient , public Susi::System::Component {
        public:
            ApiClientComponent(std::string addr) : ApiClient{addr} {}
            virtual void start() override {
                LOG(INFO) << "started api-client";
            }
            virtual void stop() override {
                close();
            }
        };

    }
}

#endif // __APICLIENT__
