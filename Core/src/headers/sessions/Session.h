/*
 * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 *
 * @author: Christian Sonderfeld (christian.sonderfeld@webvariants.de), Thomas Krause (thomas.krause@webvariants.de)
 */

#include <map>
#include <Poco/Timestamp.h>
#include <chrono>
#include <iostream>

#include "util/Any.h"

namespace Susi {
    class Session {
    protected:
        Poco::Timestamp deadline;
        std::map<std::string, Susi::Util::Any> attributes;
        std::map<std::string, std::deque<Susi::Util::Any>> multiAttributes;

    public:
        Session() : Session( std::chrono::milliseconds( 10000 ) ) {}
        Session( std::chrono::milliseconds milliseconds );
        Session( const Session & other ) :
            deadline {other.deadline},
                 attributes {other.attributes},
        multiAttributes {other.multiAttributes} {}
        Session( Session && other ) {
            *this = std::move( other );
        }
        Susi::Session& operator=( const Susi::Session & other ) {
            deadline = other.deadline;
            attributes = other.attributes;
            multiAttributes = other.multiAttributes;
            return *this;
        }

        Susi::Session& operator=( Susi::Session && other ) {
            std::swap( deadline,other.deadline );
            std::swap( attributes,other.attributes );
            std::swap( multiAttributes,other.multiAttributes );
            other.attributes.clear();
            other.multiAttributes.clear();
            return *this;
        }
        bool isDead();
        void addTime( std::chrono::milliseconds milliseconds );
        bool setAttribute( std::string key, Susi::Util::Any value );
        void pushAttribute( std::string key, Susi::Util::Any value );
        bool removeAttribute( std::string key );
        Susi::Util::Any getAttribute( std::string key );
        bool die();
    };
}
