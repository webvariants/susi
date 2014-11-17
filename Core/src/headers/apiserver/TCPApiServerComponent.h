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

#ifndef __TCPAPISERVERCOMPONENT__
#define __TCPAPISERVERCOMPONENT__

#include "Poco/Net/TCPServer.h"
#include "Poco/Net/TCPServerConnectionFactory.h"
#include "Poco/Net/TCPServerConnection.h"
#include "Poco/Net/Socket.h"
#include "Poco/Net/SocketReactor.h"
#include "Poco/Net/SocketAcceptor.h"
#include "Poco/Net/SocketNotification.h" 
#include "Poco/NObserver.h"
#include "logger/easylogging++.h"
#include "apiserver/ApiServerComponent.h"
#include "apiserver/JSONStreamCollector.h"
#include "world/BaseComponent.h"

namespace Susi {
    namespace Api {

        class TCPApiServerComponent : public Susi::System::BaseComponent {
        protected:

            class ConnectionHandler {
            public:
                ConnectionHandler(Poco::Net::StreamSocket& socket, Poco::Net::SocketReactor& reactor):
                    _socket(socket),
                    _reactor(reactor),
                    _pBuffer{new char[BUFFER_SIZE]},
                    _collector{[this](std::string & msg){
                        auto data = Susi::Util::Any::fromJSONString(msg);
                        _api->onMessage(_sessionID,data);
                    }}
                {
                    _reactor.addEventHandler(_socket, Poco::NObserver<ConnectionHandler, Poco::Net::ReadableNotification>(*this, &ConnectionHandler::onReadable));
                    _reactor.addEventHandler(_socket, Poco::NObserver<ConnectionHandler, Poco::Net::ShutdownNotification>(*this, &ConnectionHandler::onShutdown));
                }

                ~ConnectionHandler() {
                    _reactor.removeEventHandler(_socket, Poco::NObserver<ConnectionHandler, Poco::Net::ReadableNotification>(*this, &ConnectionHandler::onReadable));
                    _reactor.removeEventHandler(_socket, Poco::NObserver<ConnectionHandler, Poco::Net::ShutdownNotification>(*this, &ConnectionHandler::onShutdown));
                    _api->unregisterSender(_sessionID);
                    _api->onClose(_sessionID);
                }

                void onReadable(const Poco::AutoPtr<Poco::Net::ReadableNotification>& pNf) {
                    int n = _socket.receiveBytes(_pBuffer, BUFFER_SIZE);
                    if (n > 0){
                        std::string data{_pBuffer,static_cast<size_t>(n)};
                        _collector.collect(data);
                    }else{
                        delete this;
                    }
                }

                void onShutdown(const Poco::AutoPtr<Poco::Net::ShutdownNotification>& pNf) {
                    delete this;
                }

                void setApi(std::shared_ptr<Susi::Api::ApiServerComponent> api){
                    _api = api;
                }

                void setSessionID(std::string id){
                    _sessionID = id;
                }

                Poco::Net::StreamSocket& socket(){
                    return _socket;
                }

            private:
                enum
                {
                    BUFFER_SIZE = 1024
                };

                std::shared_ptr<Susi::Api::ApiServerComponent> _api;
                std::string _sessionID;
                Poco::Net::StreamSocket   _socket;
                Poco::Net::SocketReactor& _reactor;
                char*                     _pBuffer;
                Susi::Api::JSONStreamCollector _collector;
            };
            
            class Acceptor : public Poco::Net::SocketAcceptor<ConnectionHandler> {
            public:
                Acceptor(Poco::Net::ServerSocket & svs, Poco::Net::SocketReactor & reactor, std::shared_ptr<Susi::Api::ApiServerComponent> api) :
                    Poco::Net::SocketAcceptor<ConnectionHandler>{svs,reactor},
                    _api{api} {}
            protected:
                std::shared_ptr<ApiServerComponent> _api;
                virtual ConnectionHandler* createServiceHandler(Poco::Net::StreamSocket & socket) override{
                    auto ptr = new ConnectionHandler{socket,*(reactor())};
                    Poco::Timestamp now;
                    std::string sessionID = std::to_string( now.epochMicroseconds() );
                    ptr->setApi(_api);
                    ptr->setSessionID(sessionID);
                    _api->onConnect(sessionID);
                    _api->registerSender(sessionID,[ptr](Susi::Util::Any & data){
                        std::string msg = data.toJSONString()+"\n";
                        ptr->socket().sendBytes(msg.c_str(),msg.size());
                    });
                    return ptr;
                }
            };

            std::shared_ptr<Susi::Api::ApiServerComponent> api;
            std::shared_ptr<Susi::Events::IEventSystem> eventsystem;
            Poco::Net::SocketAddress address;
            Poco::Net::ServerSocket serverSocket;
            Poco::Net::SocketReactor reactor;
            Acceptor acceptor;
            std::thread runloop;

        public:
            TCPApiServerComponent( Susi::System::ComponentManager * mgr,
                                   std::string addr ) :
                Susi::System::BaseComponent {mgr},
                 api {componentManager->getComponent<Susi::Api::ApiServerComponent>( "apiserver" )},
                 eventsystem {componentManager->getComponent<Susi::Events::IEventSystem>( "eventsystem" )},
                 address {addr},
                 serverSocket {address},
                 acceptor{serverSocket,reactor,api}
            {}

            virtual void start() override {
                runloop = std::move(std::thread{[this](){
                    reactor.run();
                }});
                std::string msg {"started TCPApiServerComponent on "};
                msg += address.toString();
                LOG(INFO) <<  msg ;
                std::this_thread::sleep_for( std::chrono::milliseconds {250} );
            }
            virtual void stop() override {
                reactor.stop();
                if(runloop.joinable())runloop.join();
                serverSocket.close();
            }
            ~TCPApiServerComponent() {
                stop();
                LOG(INFO) <<  "stopped TCPApiServerComponent" ;
            }

        };

    }
}

#endif // __TCPAPISERVERCOMPONENT__
