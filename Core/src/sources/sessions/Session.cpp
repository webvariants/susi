/*
 * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 *
 * @author: Christian Sonderfeld (christian.sonderfeld@webvariants.de)
 */

#include "sessions/Session.h"

namespace Susi {
    Session::Session( std::chrono::milliseconds milliseconds ) {
        addTime( milliseconds );
    }
    bool Session::isDead() {
        Poco::Timestamp now;
        //std::cout<<deadline.epochTime()<<" : "<<now.epochTime()<<std::endl;
        if( deadline <= now ) return true;
        return false;
    }
    void Session::addTime( std::chrono::milliseconds milliseconds ) {
        deadline += std::chrono::duration_cast<std::chrono::microseconds>( milliseconds ).count();
    }
    void Session::pushAttribute( std::string key, Susi::Util::Any value ) {
        if( attributes.count( key ) > 0 ) {
            multiAttributes[key].push_back( attributes[key] );
            multiAttributes[key].push_back( value );
            attributes.erase( key );
        }
        else if( multiAttributes.count( key ) > 0 ) {
            multiAttributes[key].push_back( value );
        }
        else {
            attributes[key] = Susi::Util::Any( value );
        }
    }
    bool Session::removeAttribute( std::string key ) {
        if( attributes.count( key ) > 0 ) {
            attributes.erase( key );
            return true;
        }
        if( multiAttributes.count( key ) > 0 ) {
            multiAttributes.erase( key );
            return true;
        }
        return false;
    }
    Susi::Util::Any Session::getAttribute( std::string key ) {
        if( key.length() > 0 ) {
            if( attributes.count( key ) > 0 ) {
                return attributes[key];
            }
            if( multiAttributes.count( key ) > 0 ) {
                return multiAttributes[key];
            }
        }
        return Susi::Util::Any();
    }
    bool Session::setAttribute( std::string key, Susi::Util::Any value ) {
        if( key.length() > 0 ) {
            attributes[key] = value;
            return true;
        }
        return false;
    }
    bool Session::die() {
        deadline = Poco::Timestamp();
        return true;
    }
}
