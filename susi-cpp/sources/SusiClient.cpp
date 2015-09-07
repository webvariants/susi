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

#include "susi/SusiClient.h"


void Susi::SusiClient::onFrame(std::string & frame) {
    BSON::Value packet = BSON::Value::fromJSON(frame);
    if (packet.isObject()) {
        if (packet["type"].isString() && packet["data"].isObject()) {
            const std::string & type = packet["type"];
            if (type == "ack") {
                onAck(packet["data"]);
            }
            if (type == "dismiss") {
                onDismiss(packet["data"]);
            }
            if (type == "consumerEvent" && packet["data"]["topic"].isString()) {
                onConsumerEvent(packet["data"]);
            }
            if (type == "processorEvent" && packet["data"]["topic"].isString()) {
                onProcessorEvent(packet["data"]);
            }
        }
    }
}

void Susi::SusiClient::onAck(BSON::Value & event) {
    if (event["id"].isString()) {
        const std::string & id = event["id"];
        if (finishCallbacks.count(id)) {
            auto evt = std::make_shared<Event>(event);
            finishCallbacks[id](evt);
            finishCallbacks.erase(id);
        }
    }
}

void Susi::SusiClient::onDismiss(BSON::Value & event) {
    if (event["id"].isString()) {
        const std::string & id = event["id"];
        if (finishCallbacks.count(id)) {
            auto evt = std::make_shared<Event>(event);
            evt->setDismissedHeader();
            finishCallbacks[id](evt);
            finishCallbacks.erase(id);
        }
    }
}

void Susi::SusiClient::onConsumerEvent(BSON::Value & event) {
    auto evt = eventmanager.createEvent(event);
    eventmanager.publish(std::move(evt), [this](SharedEventPtr evt) {
        BSON::Value packet = BSON::Object{
            {"type", "ack"},
            {"data", evt->toAny()}
        };
        if (isConnected) {
            send(packet.toJSON());
        } else {
            messageQueue.emplace_back(new BSON::Value{packet});
        }
    }, false, true);
}

void Susi::SusiClient::onProcessorEvent(BSON::Value & event) {
    auto evt = eventmanager.createEvent(event);
    eventmanager.publish(std::move(evt), [this](SharedEventPtr evt) {
        BSON::Value packet = BSON::Object{
            {"type", evt->hasDismissedHeader() ? "dismiss" : "ack"},
            {"data", evt->toAny()}
        };
        if (isConnected) {
            send(packet.toJSON());
        } else {
            messageQueue.emplace_back(new BSON::Value{packet});
        }
    }, true, false);
}


void Susi::SusiClient::sendRegisterProcessor(std::string topic) {
    BSON::Value packet = BSON::Object{
        {"type", "registerProcessor"},
        {
            "data", BSON::Object{
                {"topic", topic}
            }
        }
    };
    send(packet.toJSON());
}

void Susi::SusiClient::sendRegisterConsumer(std::string topic) {
    BSON::Value packet = BSON::Object{
        {"type", "registerConsumer"},
        {
            "data", BSON::Object{
                {"topic", topic}
            }
        }
    };
    send(packet.toJSON());
}

void Susi::SusiClient::onConnect() {

    isConnected = true;

    for (auto & kv : registerProcessorCounter) {
        sendRegisterProcessor(kv.first);
    }

    for (auto & kv : registerConsumerCounter) {
        sendRegisterConsumer(kv.first);
    }

    while (!messageQueue.empty()) {
        send(messageQueue.front()->toJSON());
        messageQueue.pop_front();
    }
}

void Susi::SusiClient::onClose() {
    isConnected = false;
}

void Susi::SusiClient::publish(EventPtr event, Consumer finishCallback) {
    std::string id = event->id;
    BSON::Value packet = BSON::Object{
        {"type", "publish"},
        {"data", event->toAny()}
    };
    if (finishCallback) {
        finishCallbacks[id] = finishCallback;
    }
    if (isConnected) {
        send(packet.toJSON());
    } else {
        messageQueue.emplace_back(new BSON::Value{packet});
    }
}

std::uint64_t Susi::SusiClient::registerProcessor(std::string topic, Processor processor) {
    auto id = eventmanager.registerProcessor(topic, processor);
    registerProcessorCounter[topic]++;
    if (registerProcessorCounter[topic] == 1 && isConnected) {
        sendRegisterProcessor(topic);
    }
    return id;
}

std::uint64_t Susi::SusiClient::registerConsumer(std::string topic, Consumer consumer) {
    auto id = eventmanager.registerConsumer(topic, consumer);
    registerConsumerCounter[topic]++;
    if (registerConsumerCounter[topic] == 1 && isConnected) {
        sendRegisterConsumer(topic);
    }
    return id;
}

bool Susi::SusiClient::unregisterProcessor(std::uint64_t id) {
    return eventmanager.unregister(id);
}

bool Susi::SusiClient::unregisterConsumer(std::uint64_t id) {
    return eventmanager.unregister(id);
}

std::uint64_t Susi::SusiClient::generateId() {
    return std::chrono::system_clock::now().time_since_epoch().count();
}


