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

#ifndef __JSONSTREAMCOLLECTOR__
#define __JSONSTREAMCOLLECTOR__

#include <map>
#include <sstream>
#include <functional>
#include "util/Any.h"

#include <iostream>

namespace Susi {
    namespace Api {

        class JSONStreamCollector {
        protected:
            std::string message = "";
            size_t opening = 0;
            size_t closing = 0;
            std::function<void( std::string& )> _onMessage;
        public:
            JSONStreamCollector( std::function<void( std::string& )> onMessage ) : _onMessage {onMessage} {}
            JSONStreamCollector() {}
            void operator=( const JSONStreamCollector & other ) {
                _onMessage = other._onMessage;
            };
            void collect( std::string & data ) {
                for( size_t i=0; i<data.size(); i++ ) {
                    char c = data[i];
                    message += c;
                    if( c=='{' )opening++;
                    if( c=='}' ) {
                        closing++;
                        if( opening==closing ) {
                            std::string msg = message;
                            message = "";
                            opening=0;
                            closing=0;
                            if( _onMessage ) {
                                _onMessage( msg );
                            }
                            else {
                                std::cout<<"no function!"<<std::endl;
                            }
                        }
                    }
                }
            }
        };

    }
}

#endif // __JSONSTREAMCOLLECTOR__