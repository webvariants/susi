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

#ifndef __COAPSERVERCOMPONENT__
#define __COAPSERVERCOMPONENT__

#include "Poco/Net/DatagramSocket.h"
#include "Poco/Net/SocketAddress.h"

#include "susi/logger/easylogging++.h"
#include "susi/world/BaseComponent.h"

#include <atomic>
#include <thread>

namespace Susi {
    namespace Api {

        class CoapServerComponent : public Susi::System::BaseComponent {
        protected:
            std::thread _runloop;
            std::atomic<bool> _stop{false};
            Poco::Net::SocketAddress _address;
            Poco::Net::DatagramSocket _serverSocket;
        public:
            CoapServerComponent( Susi::System::ComponentManager * mgr,
                                   std::string addr ) :
                Susi::System::BaseComponent {mgr},
                 _address {addr},
                 _serverSocket {_address,true}
            {}

            virtual void start() override {
                _runloop = std::move(std::thread{[this](){
                    Poco::Net::SocketAddress requestAddr;
                    Poco::Timespan span{250000};
                    while(!_stop.load()) {
                        if (_serverSocket.poll(span, Poco::Net::Socket::SELECT_READ)) {
                            try {
                                char buffer[1<<16];
                                int n = _serverSocket.receiveFrom(buffer, sizeof(buffer), requestAddr);
                                n = _serverSocket.sendTo(buffer, n, requestAddr);
                            }catch (Poco::Exception& exc) {
                                LOG(ERROR) << exc.displayText();
                            }
                        }
                    }
                }});
                LOG(INFO) << "started CoapServerComponent.";
            }
            virtual void stop() override {
                _stop.store(true);
                if(_runloop.joinable())_runloop.join();
                _serverSocket.close();
            }
            ~CoapServerComponent() {
                stop();
                LOG(INFO) <<  "stopped CoapServerComponent." ;
            }

        };

    }
}

#endif // __COAPSERVERCOMPONENT__
