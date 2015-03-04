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

#ifndef __AUTODISCOVERYMANAGER__
#define __AUTODISCOVERYMANAGER__

#include <Poco/Net/SocketAddress.h>
#include <Poco/Net/MulticastSocket.h>

#include <thread>
#include <atomic>
#include <vector>

namespace Susi {
    namespace Autodiscovery {

        class Manager {
        protected:
            Poco::Net::SocketAddress _groupAddr;
            Poco::Net::NetworkInterface _interface;
            Poco::Net::MulticastSocket _socket;

            std::string _ownAddr;

            std::vector<std::string> _onlineList;

            std::thread _runloop;
            std::atomic<bool> _stop {false};

            Poco::Net::NetworkInterface findInterface() {
                Poco::Net::NetworkInterface::Map m = Poco::Net::NetworkInterface::map();
                for( Poco::Net::NetworkInterface::Map::const_iterator it = m.begin(); it != m.end(); ++it ) {
                    if( it->second.supportsIPv4() &&
                            it->second.firstAddress( Poco::Net::IPAddress::IPv4 ).isUnicast() &&
                            !it->second.isLoopback() &&
                            !it->second.isPointToPoint() )
                    {
                        return it->second;
                    }
                }
                return Poco::Net::NetworkInterface();
            }

            virtual void onNewHost( std::string & addr ) = 0;

            void send( std::string & message ) {
                try {
                    _socket.sendTo( message.c_str(),message.size(), _groupAddr );
                }
                catch( Poco::Exception& exc ) {
                    std::cerr << "Multicast: " << exc.displayText() << std::endl;
                }
            }

            void sendHello() {
                std::string message {"hello "+_ownAddr};
                send( message );
            }
            void sendBye( std::string addr ) {
                std::string message {"bye "+addr};
                send( message );
            }

        public:
            Manager( std::string groupAddr, std::string ownAddr ) :
                _groupAddr {groupAddr},
                       _interface {findInterface()},
            _ownAddr {ownAddr}
            {
                _socket.bind( Poco::Net::SocketAddress{Poco::Net::IPAddress(), _groupAddr.port()}, true );
                _socket.joinGroup( _groupAddr.host(), _interface );
            }

            void start() {
                _runloop = std::move( std::thread {[this]() {
                    Poco::Timespan span( 250000 );
                    sendHello();
                    while( !_stop.load() ) {
                        if( _socket.poll( span, Poco::Net::Socket::SELECT_READ ) ) {
                            try {
                                char buffer[256];
                                Poco::Net::SocketAddress sender;
                                int n = _socket.receiveFrom( buffer, sizeof( buffer )-1, sender );
                                buffer[n] = 0;
                                std::string message {buffer};
                                if( message.find( "hello" )==0 ) {
                                    std::string senderAddr = message.substr( message.find( " " )+1 );
                                    bool allreadyKnown = false;
                                    for( auto & knownAddr : _onlineList ) {
                                        if( knownAddr == senderAddr ) {
                                            allreadyKnown = true;
                                            break;
                                        }
                                    }
                                    if( !allreadyKnown && senderAddr != _ownAddr ) {
                                        _onlineList.push_back( senderAddr );
                                        sendHello();
                                        onNewHost( senderAddr );
                                    }
                                }
                            }
                            catch( Poco::Exception& exc ) {
                                std::cerr << "Multicast: " << exc.displayText() << std::endl;
                            }
                        }
                    }
                }
                                                  } );
            }

            void stop() {
                _stop.store( true );
                if( _runloop.joinable() )_runloop.join();
            }

            ~Manager() {
                stop();
            }

        };

    }
}

#endif // __AUTODISCOVERYMANAGER__
