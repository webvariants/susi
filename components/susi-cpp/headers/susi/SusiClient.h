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
#include "susi/LineFramer.h"
#include "susi/EventManager.h"

#include <iostream>
#include <functional>
#include <chrono>
#include <regex>

#include "SSLClient.h"
#define Client SSLClient

namespace Susi {

class SusiClient : public FramingClient<LineFramer, Client> {
  public:
    SusiClient(std::string host, short port, std::string keyFile, std::string certificateFile) :
        FramingClient<LineFramer, Client> {host, port, keyFile, certificateFile} {}

    virtual ~SusiClient() {}

    void publish(EventPtr event, Consumer finishCallback = Consumer{});

    std::uint64_t registerProcessor(std::string topic, Processor processor);
    std::uint64_t registerConsumer(std::string topic, Consumer consumer);
    bool unregisterProcessor(std::uint64_t id);
    bool unregisterConsumer(std::uint64_t id);

    EventPtr createEvent(std::string topic) {
        return eventmanager.createEvent(topic);
    }
    EventPtr createEvent(BSON::Value & event) {
        return eventmanager.createEvent(event);
    }

    void ack(EventPtr evt) {
        eventmanager.ack(std::move(evt));
    }

    void dismiss(EventPtr evt) {
        eventmanager.dismiss(std::move(evt));
    }

    void login(const std::string & username, const std::string & password);
    void logout();

  protected:
    EventManager eventmanager;

    std::atomic<bool> isConnected{false};
    std::string token;
    std::regex authRequiredRegex;

    std::deque<std::shared_ptr<BSON::Value>> messageQueue;
    std::map<std::string, int> registerProcessorCounter;
    std::map<std::string, int> registerConsumerCounter;

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
    std::map<std::string, Consumer> finishCallbacks;

};

}

#endif // __SUSICLIENT__
