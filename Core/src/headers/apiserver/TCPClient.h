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

#ifndef __POCOTCPCLIENT__
#define __POCOTCPCLIENT__

#include <Poco/Net/StreamSocket.h>
#include <Poco/Net/SocketAddress.h>
#include <thread>
#include <iostream>
#include <atomic>
#include "logger/easylogging++.h"

namespace Susi {
    namespace Api {

        class TCPClient {
        public:

            //These functions are not pure virtual, because of destruction problems.
            virtual void onConnect() {};
            virtual void onReconnect() {onConnect();};
            virtual void onData( std::string & data ) {};
            virtual void onClose() {};

            TCPClient( std::string address );
            
            TCPClient( TCPClient && other );


            void close() ;
            void send( std::string msg ) ;
            void join() ;
            virtual ~TCPClient() ;

            void setMaxReconnectCount(size_t count) {
                maxReconnectCount = count;
            }

        private:

            void startRunloop();

            size_t maxReconnectCount = 0;

            std::atomic<bool> isClosed{false};
            Poco::Net::SocketAddress sa;
            Poco::Net::StreamSocket sock;
            std::thread runloop;

            std::string sendbuff;
        };

    }
}

#endif // __POCOTCPCLIENT__
