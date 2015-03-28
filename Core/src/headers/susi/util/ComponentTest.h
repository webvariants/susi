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

#ifndef __COMPONENTTEST__
#define __COMPONENTTEST__

#include "gtest/gtest.h"
#include "susi/config/Config.h"
#include "susi/iocontroller/IOController.h"
#include "susi/logger/easylogging++.h"
#include "susi/events/IEventSystem.h"
#include "susi/world/SusiServerComponentManager.h"

class ComponentTest : public ::testing::Test {
public:
    std::string base_path;

    ComponentTest() {
        

        base_path = Poco::Path( Poco::Path::current() ).toString() + "component_test";

        BSON::Object cfg = BSON::Value::fromJSON( getConfigString( base_path ) );
        componentManager.reset(dynamic_cast<Susi::System::ComponentManager*>(new Susi::System::SusiServerComponentManager{cfg}));
        eventManager = componentManager->getComponent<Susi::Events::IEventSystem>( "eventsystem" );


        io.makeDir( base_path );
    }

    virtual ~ComponentTest() {
        io.deletePath( base_path );
    }

protected:
    std::shared_ptr<Susi::System::ComponentManager> componentManager;
    std::shared_ptr<Susi::Events::IEventSystem> eventManager;

    Susi::IOController io;

    virtual void GoodCases() = 0;
    virtual void BadCases() = 0;
    virtual void EdgeCases() = 0;

    std::string getConfigString( std::string path ) {
        std::string configString ="{"
                                  "\"eventsystem\" : {"
                                  "\"threads\": 4,"
                                  "\"queuelen\": 32"
                                  "},"
                                  "\"heartbeat\": {"
                                  "\"fix\":\"me\""
                                  "},"
                                  "\"dbmanager\": ["
                                  "{"
                                  "\"identifier\" : \"auth\","
                                  "\"type\": \"sqlite3\","
                                  "\"uri\" : \""+path+"/auth.sqlite3\""
                                  "},"
                                  "{"
                                  "\"identifier\" : \"test_db\","
                                  "\"type\": \"sqlite3\","
                                  "\"uri\" : \""+path+"/test_db.sqlite3\""
                                  "}"
                                  "],"
                                  "\"authcontroller\": {"
                                  "\"db\": \"auth\""
                                  "},"
                                  "\"tcpapiserver\": {"
                                  "\"address\": \"[::1]:4000\","
                                  "\"threads\": 4,"
                                  "\"backlog\": 32"
                                  "},"
                                  "\"enginestarter\": {"
                                  "\"path\": \"./enginestarter_test\""
                                  "},"
                                  "\"iocontroller\": {},"
                                  "\"sessionmanager\": {"
                                  "\"lifetime\": 1000"
                                  "},"
                                  "\"statecontroller\": {"
                                  "\"file\": \""+path+"/states.json\""
                                  "},"
                                  "\"syscallcontroller\": {"
                                  "\"threads\": 4,"
                                  "\"queuelen\": 16,"
                                  "\"commands\": {"
                                  "\"TEST1\": \"echo foo\","
                                  "\"TEST2\": \"echo foo 1>&2\","
                                  "\"TEST3\": \"echo $arg\","
                                  "\"PWD\"  : \"pwd\""
                                  "}"
                                  "},"
                                  "\"httpserver\": {"
                                  "\"address\": \"[::1]:8080\","
                                  "\"assets\": \""+path+"/assets\","
                                  "\"upload\": \""+path+"/upload\""
                                  "},"
                                  "\"apiserver\": {},"
                                  "\"logger\": {\"easylogging++\": \"../../Core/easylogging++.conf\"}"
                                  "}";

        return configString;
    }

    long subscribe( std::string topic, Susi::Events::Processor processor ) {
        long id = eventManager->subscribe( topic,std::move( processor ));
        return id;
    }
    /*long subscribe( Susi::Events::Predicate pred, Susi::Events::Processor processor ) {
        long id = eventManager->subscribe( pred,std::move( processor ));
        return id;
    }*/
    long subscribe( std::string topic, Susi::Events::Consumer consumer ) {
        long id = eventManager->subscribe( topic,std::move( consumer ));
        return id;
    }
    /*long subscribe( Susi::Events::Predicate pred, Susi::Events::Consumer consumer ) {
        long id = eventManager->subscribe( pred,std::move( consumer ));
        return id;
    }*/
    bool unsubscribe( long id ) {
        return eventManager->unsubscribe( id );
    }
    void publish( Susi::Events::EventPtr event, Susi::Events::Consumer finishCallback = Susi::Events::Consumer {} ) {
        eventManager->publish( std::move( event ),finishCallback );
    }
    Susi::Events::EventPtr createEvent( std::string topic ) {
        return eventManager->createEvent( topic );
    }

    Susi::Events::SharedEventPtr waitForConsumerEvent( std::string topic,std::chrono::milliseconds timeout ) {
        callbackCalled = false;
        sharedEvent.reset();
        Susi::Events::Consumer consumer = [this]( Susi::Events::SharedEventPtr event ) {
            onConsumerEvent( event );
        };
        long id = subscribe( topic,consumer );
        {
            std::unique_lock<std::mutex> lock {mutex};
            cv.wait_for( lock,timeout,[this]() {
                return callbackCalled;
            } );
        }
        unsubscribe( id );
        return sharedEvent;
    }


    Susi::Events::EventPtr waitForProcessorEvent( std::string topic,std::chrono::milliseconds timeout ) {
        callbackCalled = false;
        event.reset();
        Susi::Events::Processor processor = [this]( Susi::Events::EventPtr event ) {
            onProcessorEvent( std::move( event ) );
        };
        long id = subscribe( topic,processor );
        {
            std::unique_lock<std::mutex> lock {mutex};
            cv.wait_for( lock,timeout,[this]() {
                return callbackCalled;
            } );
        }
        unsubscribe( id );
        return std::move( event );
    }

    Susi::Events::SharedEventPtr publish_sync( Susi::Events::EventPtr event ) {
        Susi::Events::SharedEventPtr result;
        std::condition_variable cv;
        bool called = false;
        std::mutex mutex;
        publish( std::move( event ),[&cv, &called, &result]( Susi::Events::SharedEventPtr evt ) {
            result = evt;
            called = true;
            cv.notify_one();
        } );
        {
            std::unique_lock<std::mutex> lock {mutex};
            cv.wait( lock,[&called]() {
                return called;
            } );
        }
        return result;
    }


    bool hasErrorHeader( Susi::Events::SharedEventPtr event ) {
        auto headers = event->getHeaders();
        bool error_found = false;

        for( size_t i=0; i<headers.size(); ++i ) {
            if( headers[i].first == "error" ) {
                error_found = true;
                break;
            }
        }

        return error_found;
    }

    int countSetCookieInHeaders( std::vector<std::pair<std::string,std::string>> headers ) {
        int set_cookie_count = 0;

        for( size_t i=0; i<headers.size(); ++i ) {
            if( headers[i].first == "Set-Cookie" ) {
                set_cookie_count++;
            }
        }

        return set_cookie_count;
    }

    std::string getSessionIDFromHeaders( std::vector<std::pair<std::string,std::string>> headers ) {

        int last_set_cookie_idx = -1;
        std::string sessionID = "";

        for( size_t i=0; i<headers.size(); ++i ) {
            if( headers[i].first == "Set-Cookie" ) {
                last_set_cookie_idx = i;
            }
        }

        if( last_set_cookie_idx > -1 ) {
            std::string splitStr = headers[last_set_cookie_idx].second;
            std::vector<std::string> elems_1;
            std::vector<std::string> elems_2;
            Susi::Util::Helpers::split( splitStr, ';', elems_1 );
            Susi::Util::Helpers::split( elems_1[0], '=', elems_2 );
            sessionID = elems_2[1];
        }

        return sessionID;
    }

private:
    Susi::Events::EventPtr event;
    Susi::Events::SharedEventPtr sharedEvent;
    std::condition_variable cv;
    std::mutex mutex;
    bool callbackCalled;

    void onConsumerEvent( Susi::Events::SharedEventPtr evt ) {
        sharedEvent = evt;
        callbackCalled = true;
        cv.notify_one();
    }
    void onProcessorEvent( Susi::Events::EventPtr evt ) {
        event = std::move( evt );
        callbackCalled = true;
        cv.notify_one();
    }


};



#endif // __COMPONENTTEST__

