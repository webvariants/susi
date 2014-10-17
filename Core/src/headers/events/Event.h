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

#ifndef __EVENT__
#define __EVENT__

#include "util/Any.h"
#include <chrono>

namespace Susi {
    namespace Events {

        typedef std::pair<std::string,std::string> Header;

        class Event {
        protected:
        public:
            std::string id;
            char authlevel = 0;
            std::string topic;
            std::vector<Header> headers;
            Susi::Util::Any payload;
            std::string sessionID;
            Event() {
                id = std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
            }
            Event( std::string topic_ ) {
                id = std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
                topic = topic_;
            }
            Event( const Event & other ) {
                id = other.id;
                topic = other.topic;
                headers = other.headers;
                payload = other.payload;
                sessionID = other.sessionID;
                authlevel = other.authlevel;
            }
            Event& operator=( Event & other ) {
                setID( other.getID() );
                setSessionID( other.getSessionID() );
                setTopic( other.getTopic() );
                setHeaders( other.getHeaders() );
                setPayload( other.getPayload() );
                setAuthlevel( other.getAuthlevel() );
                return *this;
            }
            Event( Susi::Util::Any & any ) {
                if( !any["id"].isNull() ) {
                    id = static_cast<std::string>( any["id"] );
                }
                else {
                    id = std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
                }
                if( !any["topic"].isNull() ) {
                    topic = static_cast<std::string>( any["topic"] );
                }
                if( !any["headers"].isNull() ) {
                    Susi::Util::Any::Array headers = any["headers"];
                    for( Susi::Util::Any::Object & obj : headers ) {
                        for( auto & kv : obj ) {
                            this->headers.push_back( std::make_pair( kv.first,static_cast<std::string>( kv.second ) ) );
                        }
                    }
                }
                if( !any["payload"].isNull() ) {
                    payload = any["payload"];
                }
                if( !any["sessionid"].isNull() ) {
                    sessionID = static_cast<std::string>( any["sessionid"] );
                }
                if( !any["authlevel"].isNull() ) {
                    authlevel = static_cast<char>( any["authlevel"] );
                }
            }
            Susi::Util::Any toAny() {
                Susi::Util::Any obj = Susi::Util::Any::Object {
                    {"id",id},
                    {"sessionid",sessionID},
                    {"topic",topic},
                    {"authlevel",authlevel}
                };
                if( !payload.isNull() ) {
                    obj["payload"] = payload;
                }
                for( size_t i=0; i<headers.size(); ++i ) {
                    Susi::Util::Any::Object header {
                        {headers[i].first,headers[i].second}
                    };
                    obj["headers"].push_back( Susi::Util::Any {header} );
                }
                return obj;
            }

            inline std::string getID() {
                return id;
            }
            inline std::string getSessionID() {
                return sessionID;
            }
            inline std::string getTopic() {
                return topic;
            }
            inline std::vector<Header> & getHeaders() {
                return headers;
            }
            inline char getAuthlevel() {
                return authlevel;
            }
            inline Susi::Util::Any & getPayload() {
                return payload;
            }

            inline void setID( std::string _id ) {
                id = _id;
            }
            inline void setSessionID( std::string _sessionID ) {
                sessionID = _sessionID;
            }
            inline void setTopic( std::string _topic ) {
                topic = _topic;
            }
            inline void setHeaders( std::vector<Header> _headers ) {
                headers = _headers;
            }
            inline void setPayload( Susi::Util::Any _payload ) {
                payload = _payload;
            }
            inline void setAuthlevel( char _authlevel ) {
                authlevel = _authlevel;
            }

            std::string toString() {
                return toAny().toJSONString();
            }


        };

    }
}

#endif // __EVENT__
