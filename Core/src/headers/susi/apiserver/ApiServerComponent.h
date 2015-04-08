/*
 * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 *
 * @author: Tino Rusch (tino.rusch@webvariants.de), Thomas Krause (thomas.krause@webvariants.de)
 */

#ifndef __API_SERVER_COMPONENT__
#define __API_SERVER_COMPONENT__

#include <memory>
#include <functional>
#include <mutex>
#include "susi/events/EventManagerComponent.h"
#include "susi/sessions/SessionManagerComponent.h"

#include "susi/world/SessionAwareComponent.h"

namespace Susi {
    namespace Api {

        class ApiServerComponent : public Susi::System::SessionAwareComponent {
        protected:

            std::map<std::string,std::function<void( BSON::Value& )>> senders;
            std::recursive_mutex sendersMutex;
            std::map<std::string,std::map<std::string,long>> consumerSubscriptions;
            std::recursive_mutex consumerMutex;
            std::map<std::string,std::map<std::string,long>> processorSubscriptions;
            std::recursive_mutex processorMutex;
            std::map<std::string,std::map<std::string,Susi::Events::EventPtr>> eventsToAck; //sessionid -> eventid -> event
            std::recursive_mutex eventsMutex;

            void handleRegisterConsumer( std::string & id, BSON::Value & packet );
            void handleRegisterProcessor( std::string & id, BSON::Value & packet );
            void handleUnregisterConsumer( std::string & id, BSON::Value & packet );
            void handleUnregisterProcessor( std::string & id, BSON::Value & packet );
            void handlePublish( std::string & id, BSON::Value & packet );
            void handleAck( std::string & id, BSON::Value & packet );

            void sendOk( std::string & id );
            void sendFail( std::string & id,std::string error = "" );

            bool checkIfConfidentialHeaderMatchesSession(Susi::Events::Event & event, std::string sessionID);

            void send( std::string & id, BSON::Value & msg ) {
                std::lock_guard<std::recursive_mutex> lock {sendersMutex}; //DEADLOCK IF SENDER DEAD (websocket)
                auto & sender = senders[id];
                if( sender ){
                    try{
                        sender(msg);   
                    }catch(const std::exception & e){
                        LOG(DEBUG) << "Exception while sending. Possible timeout due to too much load?!";
                        onClose(id);
                    }
                }
            }

        public:

            ApiServerComponent( Susi::System::ComponentManager * mgr ) : Susi::System::SessionAwareComponent {mgr} {}

            ~ApiServerComponent(){
                stop();
                LOG(INFO) <<  "stopped ApiServerComponent" ;
            }

            virtual void start() override {
                LOG(INFO) <<  "started ApiServerComponent" ;
            }
            virtual void stop() override {
                unsubscribeAll();
            }

            void onConnect( std::string & id );
            void onMessage( std::string & id, BSON::Value & packet );
            void onClose( std::string & id );

            inline void registerSender( std::string id , std::function<void( BSON::Value& )> sender ) {
                std::lock_guard<std::recursive_mutex> lock {sendersMutex};
                senders[id] = sender;
            }
            inline void unregisterSender( std::string id ) {
                std::lock_guard<std::recursive_mutex> lock {sendersMutex};
                senders.erase(id);
            }

        };

    }
}

#endif // __API_SERVER_COMPONENT__