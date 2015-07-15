/*
 * Copyright (c) 2015, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 * 
 * @author: Tino Rusch (tino.rusch@webvariants.de)
 */

#ifndef __SUSICLIENT__
#define __SUSICLIENT__

#include "susi/FramingClient.h"
#include "susi/JSONFramer.h"
#include "susi/EventManager.h"

#include <iostream>
#include <functional>
#include <chrono>
#include <regex>

#ifdef WITH_SSL
    #include "SSLClient.h"
    #define Client SSLClient
#else
    #include "TCPClient.h"
    #define Client TCPClient
#endif

namespace Susi {

    class SusiClient : public FramingClient<JSONFramer,Client> {
    public:
        SusiClient(std::string host, short port) : FramingClient<JSONFramer,Client>{host,port} {}
        
        #ifdef WITH_SSL
            SusiClient(std::string host, short port, std::string keyFile, std::string certificateFile) :
                FramingClient<JSONFramer,Client>{host,port,keyFile,certificateFile} {}
        #endif
        
        virtual ~SusiClient() {}

        void publish(EventPtr event, Consumer finishCallback = Consumer{});

        std::uint64_t registerProcessor(std::string topic, Processor processor);
        std::uint64_t registerConsumer(std::string topic, Consumer consumer);
        bool unregisterProcessor(std::uint64_t id);
        bool unregisterConsumer(std::uint64_t id);

        EventPtr createEvent(std::string topic){
            return eventmanager.createEvent(topic);
        }
        EventPtr createEvent(BSON::Value & event){
            return eventmanager.createEvent(event);
        }

        void ack(EventPtr evt){
            eventmanager.ack(std::move(evt));
        }

        void dismiss(EventPtr evt){
            eventmanager.dismiss(std::move(evt));
        }

    protected:
        EventManager eventmanager;
        bool isConnected = false;
        std::deque<std::shared_ptr<BSON::Value>> messageQueue;
        std::map<std::string,int> registerProcessorCounter;
        std::map<std::string,int> registerConsumerCounter;

        void sendRegisterProcessor(std::string proc);
        void sendRegisterConsumer(std::string proc);

        virtual void onConnect() override;
        virtual void onFrame(std::string & frame) override;
        virtual void onClose() override;

        void onAck(BSON::Value & event);
        void onDismiss(BSON::Value & event);
        void onConsumerEvent(BSON::Value & event);
        void onProcessorEvent(BSON::Value & event);

        std::uint64_t generateId();

        /*std::map<std::uint64_t,std::pair<std::string,Processor>> processors;
        std::map<std::uint64_t,std::pair<std::string,Consumer>> consumers;*/
        std::map<std::string,Consumer> finishCallbacks;

    };

}

#endif // __SUSICLIENT__
