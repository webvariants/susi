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

#include "bson/Value.h"
#include <chrono>

namespace Susi {

    typedef std::pair<std::string,std::string> Header;

    class Event {
    protected:
    public:
        std::string id;
        std::string topic;
        std::vector<Header> headers;
        BSON::Value payload;
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
        }
        Event& operator=( Event & other ) {
            setID( other.getID() );
            setSessionID( other.getSessionID() );
            setTopic( other.getTopic() );
            setHeaders( other.getHeaders() );
            setPayload( other.getPayload() );
            return *this;
        }
        Event( BSON::Value & any ) {
            if( !any["id"].isUndefined() ) {
                std::string _id = any["id"];
                id = _id;
            }
            else {
                id = std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
            }
            if( !any["topic"].isUndefined() ) {
                std::string _topic = any["topic"];
                topic = _topic;
            }
            if( !any["headers"].isUndefined() ) {
                BSON::Array headers = any["headers"];
                for( BSON::Object & obj : headers ) {
                    for( auto & kv : obj ) {
                        std::string headerVal = kv.second;
                        this->headers.push_back( std::make_pair( kv.first,headerVal ) );
                    }
                }
            }
            if( !any["payload"].isUndefined() ) {
                payload = any["payload"];
            }
            if( !any["sessionid"].isUndefined() ) {
                std::string idVal = any["sessionid"];
                sessionID = idVal;
            }
           
        }
        BSON::Value toAny() {
            BSON::Value obj = BSON::Object {
                {"id",id},
                {"sessionid",sessionID},
                {"topic",topic}
            };
            if( !payload.isUndefined() ) {
                obj["payload"] = payload;
            }
            for( size_t i=0; i<headers.size(); ++i ) {
                BSON::Object header {
                    {headers[i].first,headers[i].second}
                };
                obj["headers"].push_back( BSON::Value {header} );
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
        inline BSON::Value & getPayload() {
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
        inline void setPayload( BSON::Value _payload ) {
            payload = _payload;
        }
        std::string toString() {
            return toAny().toJSON();
        }


    };

}

#endif // __EVENT__
