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
#include "events/EventManagerComponent.h"
#include "sessions/SessionManagerComponent.h"

#include "world/SessionAwareComponent.h"

namespace Susi {
    namespace Api {

        class ApiServerComponent : public Susi::System::SessionAwareComponent {
        protected:

            std::map<std::string,std::map<long,std::function<void( Susi::Util::Any& )>>> senders;
            std::map<std::string,std::map<std::string,long>> consumerSubscriptions;
            std::map<std::string,std::map<std::string,long>> processorSubscriptions;
            std::map<std::string,std::map<std::string,Susi::Events::EventPtr>> eventsToAck; //sessionid -> eventid -> event

            std::mutex mutex;

            void handleRegisterConsumer( std::string & id, Susi::Util::Any & packet );
            void handleRegisterProcessor( std::string & id, Susi::Util::Any & packet );
            void handleUnregisterConsumer( std::string & id, Susi::Util::Any & packet );
            void handleUnregisterProcessor( std::string & id, Susi::Util::Any & packet );
            void handlePublish( std::string & id, Susi::Util::Any & packet );
            void handleAck( std::string & id, Susi::Util::Any & packet );

            void sendOk( std::string & id );
            void sendFail( std::string & id,std::string error = "" );

            bool checkIfConfidentialHeaderMatchesSession(Susi::Events::Event & event, std::string sessionID);

            void send( std::string & id, Susi::Util::Any & msg ) {
                std::lock_guard<std::mutex> lock {mutex};
                auto & senderList = senders[id];
                for(auto & kv : senderList){
                    if( kv.second ) kv.second(msg);
                }
            }

        public:

            ApiServerComponent( Susi::System::ComponentManager * mgr ) : Susi::System::SessionAwareComponent {mgr} {}

            ~ApiServerComponent(){
                stop();
                Susi::Logger::info( "stopped ApiServerComponent" );
            }

            virtual void start() override {
                Susi::Logger::info( "started ApiServerComponent" );
            }
            virtual void stop() override {
                unsubscribeAll();
            }

            void onConnect( std::string & id );
            void onMessage( std::string & id, Susi::Util::Any & packet );
            void onClose( std::string & id );

            inline long registerSender( std::string id , std::function<void( Susi::Util::Any& )> sender ) {
                std::lock_guard<std::mutex> lock {mutex};
                long connId = std::chrono::system_clock::now().time_since_epoch().count();
                senders[id][connId] = sender;
                return connId;
            }
            inline void unregisterSender( std::string id, long connId ) {
                std::lock_guard<std::mutex> lock {mutex};
                auto & senderList = senders[id];
                senderList.erase(connId);
                if(senderList.size()==0){
                    onClose(id);
                }
            }

        };

    }
}

#endif // __API_SERVER_COMPONENT__