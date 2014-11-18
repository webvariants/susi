/*
 * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 *
 * @authors: Tino Rusch (tino.rusch@webvariants.de), Christian Sonderfeld (christian.sondefeld@webvariants.de), Thomas Krause (thomas.krause@webvariants.de)
 */

#include "sessions/SessionManager.h"
#include "logger/easylogging++.h"

using namespace Susi::Sessions;

void SessionManager::init( std::chrono::milliseconds stdSessionLifetime ) {
    std::lock_guard<std::mutex> lock( mutex );

    if( stdSessionLifetime.count() <= 0 ) {
        stdLifetime = std::chrono::milliseconds( 10000 );
    }
    else {
        this->stdLifetime = stdSessionLifetime;
    }
}

SessionManager::~SessionManager() {}

int SessionManager::checkSessions() {
    //std::cout<<"aquiere sessions mutex"<<std::endl;
    std::lock_guard<std::mutex> lock( mutex );
    //std::cout<<"got sessions mutex"<<std::endl;
    int deleted = 0;
    auto it = std::begin(sessions);
    while( it!=std::end( sessions ) ) {
        auto current = it++;
        if( current->second.isDead() ) {
            LOG(DEBUG) <<  "delete session "<<current->first<<" while regular checking" ;
            sessions.erase( current );
            for(auto ait = alias.begin(); ait != alias.end();){
                if(ait->second == current->first){
                    alias.erase(ait++);
                }else{
                    ++ait;
                }
            }
            deleted++;
        }
    }

    //std::cout<<"release sessions mutex"<<std::endl;


    /*std::map<std::string, Session> newSessions;
    int deleted = 0;
    for(auto & kv : sessions){
        if(!kv.second.isDead()){
            newSessions.emplace(kv.first,std::move(kv.second));
        }else{
            deleted++;
        }
    }
    sessions = std::move(newSessions);*/
    return deleted;
}

bool SessionManager::checkSession( std::string id ) {
    std::lock_guard<std::mutex> lock( mutex );
    resolveSessionID(id);
    try {
        bool alive = !sessions.at( id ).isDead();
        LOG(DEBUG) <<  "session is alive: "+std::to_string( alive ) ;
        return alive;
    }
    catch( const std::exception & e ) {
        LOG(DEBUG) <<  "session is not known" ;
        return false;
    }
}

bool SessionManager::setSessionAttribute( std::string sessionID, std::string key, Susi::Util::Any value ) {
    std::lock_guard<std::mutex> lock( mutex );
    resolveSessionID(sessionID);
    if( sessionID.length() > 0 )
    {
        if( sessions.count( sessionID ) > 0 ) {
            this->sessions[sessionID].setAttribute( key, value );
        }
        else {
            Session session = Session( this->stdLifetime );
            session.setAttribute( key, value );
            sessions[sessionID] = session;
        }
        return true;
    }
    return false;
}

bool SessionManager::pushSessionAttribute( std::string sessionID, std::string key, Susi::Util::Any value ) {
    std::lock_guard<std::mutex> lock( mutex );
    resolveSessionID(sessionID);
    if( sessionID.length() > 0 )
    {
        if( sessions.count( sessionID ) > 0 ) {
            this->sessions[sessionID].pushAttribute( key, value );
        }
        else {
            Session session = Session( this->stdLifetime );
            session.setAttribute( key, value );
            sessions[sessionID] = session;
        }
        return true;
    }
    return false;
}

bool SessionManager::removeSessionAttribute( std::string sessionID, std::string key ) {
    std::lock_guard<std::mutex> lock( mutex );
    resolveSessionID(sessionID);
    if( sessionID.length() > 0 )
    {
        if( sessions.count( sessionID ) > 0 ) {
            return this->sessions[sessionID].removeAttribute( key );
        }
    }
    return false;
}

Susi::Util::Any SessionManager::getSessionAttribute( std::string sessionID, std::string key ) {
    std::lock_guard<std::mutex> lock( mutex );
    resolveSessionID(sessionID);
    if( sessionID.length() > 0 && sessions.count( sessionID ) > 0 )
    {
        return sessions[sessionID].getAttribute( key );
    }
    return Susi::Util::Any {};
}

void SessionManager::updateSession( std::string id ) {
    std::lock_guard<std::mutex> lock( mutex );
    LOG(DEBUG) << "update session "<<id;
    resolveSessionID(id);
    LOG(DEBUG) << "session id was resolved to "<<id;
    if( sessions.count( id ) > 0 && !sessions[id].isDead() ) {
        sessions[id].addTime( stdLifetime );
    }
    else {
        LOG(DEBUG) <<  "create new session with "+std::to_string( this->stdLifetime.count() ) ;
        sessions[id] = Session( this->stdLifetime );
        sessions[id].setAttribute("authlevel",3);
    }
}

void SessionManager::updateSession( std::string id, std::chrono::milliseconds lifetime ) {
    std::lock_guard<std::mutex> lock( mutex );
    resolveSessionID(id);
    if( sessions.count( id ) > 0 ) {
        sessions[id].addTime( lifetime );
    }
    else {
        sessions[id] = Session( lifetime );
    }
}

bool SessionManager::killSession( std::string id ) {
    std::lock_guard<std::mutex> lock( mutex );
    resolveSessionID(id);
    if( sessions.count( id ) > 0 ) {
        return sessions[id].die();
    }
    return false;
}

void SessionManager::addAlias(std::string alias_, std::string sessionID){
    std::lock_guard<std::mutex> lock( mutex );
    alias[alias_] = sessionID;
}