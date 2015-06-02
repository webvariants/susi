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
    if(packet.isObject()){
        if(packet["type"].isString() && packet["data"].isObject()) {
            const std::string & type = packet["type"];  
            if(type == "ack"){
                onAck(packet["data"]);
            }
            if(type == "consumerEvent" && packet["data"]["topic"].isString()){
                onConsumerEvent(packet["data"]);
            }
            if(type == "processorEvent" && packet["data"]["topic"].isString()){
                onProcessorEvent(packet["data"]);
            }
        }
    }
}

void Susi::SusiClient::onAck(BSON::Value & event){
    if(event["id"].isString()){
        const std::string & id = event["id"];
        if(finishCallbacks.count(id)){
            finishCallbacks[id](event);
            finishCallbacks.erase(id);
        }
    }
}

void Susi::SusiClient::onConsumerEvent(BSON::Value & event){
    const std::string & topic = event["topic"];
    for(auto & kv : consumers){
        std::regex reg{kv.second.first};
        if(std::regex_match(topic,reg)){
            kv.second.second(event);
        }
    }
}

void Susi::SusiClient::onProcessorEvent(BSON::Value & event){
    const std::string & topic = event["topic"];
    for(auto & kv : processors){
        std::regex reg{kv.second.first};
        if(std::regex_match(topic,reg)){
            kv.second.second(event);
        }
    }
    BSON::Value packet = BSON::Object{
        {"type","ack"},
        {"data",event}
    };
    if(isConnected){
        send(packet.toJSON());
    }else{
        messageQueue.emplace_back(new BSON::Value{packet});
    }
}


void Susi::SusiClient::sendRegisterProcessor(std::string topic){
    BSON::Value packet = BSON::Object{
        {"type","registerProcessor"},
        {"data",BSON::Object{
            {"topic",topic}
        }}
    };
    send(packet.toJSON());
}

void Susi::SusiClient::sendRegisterConsumer(std::string topic){
    BSON::Value packet = BSON::Object{
        {"type","registerConsumer"},
        {"data",BSON::Object{
            {"topic",topic}
        }}
    };
    send(packet.toJSON());
}

void Susi::SusiClient::onConnect() {
    isConnected = true;
    

    for(auto & kv : registerProcessorCounter){
        sendRegisterProcessor(kv.first);
    }

    for(auto & kv : registerConsumerCounter){
        sendRegisterConsumer(kv.first);
    }

    while(!messageQueue.empty()){
        send(messageQueue.front()->toJSON());
        messageQueue.pop_front();
    }
}

void Susi::SusiClient::onClose() {
    isConnected = false;
}

void Susi::SusiClient::publish(std::string topic, BSON::Value payload, Consumer finishCallback){
    std::string id = std::to_string(generateId());
    BSON::Value packet = BSON::Object{
        {"type","publish"},
        {"data",BSON::Object{
            {"topic",topic},
            {"id",id},
            {"payload",payload}
        }}
    };
    if(finishCallback){
        finishCallbacks[id] = finishCallback;
    }
    if(isConnected){
        send(packet.toJSON());        
    }else{
        messageQueue.emplace_back(new BSON::Value{packet});
    }
}

std::uint64_t Susi::SusiClient::registerProcessor(std::string topic, Processor processor){
    std::uint64_t id = generateId();
    processors[id] = {topic,processor};
    registerProcessorCounter[topic]++;
    if(registerProcessorCounter[topic] == 1 && isConnected){
        sendRegisterProcessor(topic);
    }
    return id;
}

std::uint64_t Susi::SusiClient::registerConsumer(std::string topic, Consumer consumer){
    std::uint64_t id = generateId();
    consumers[id] = {topic,consumer};
    registerConsumerCounter[topic]++;
    if(registerConsumerCounter[topic] == 1 && isConnected){
        sendRegisterConsumer(topic);
    }
    return id;
}

bool Susi::SusiClient::unregisterProcessor(std::uint64_t id){
    if(processors.count(id)){
        processors.erase(id);
        return true;
    }
    return false;
}

bool Susi::SusiClient::unregisterConsumer(std::uint64_t id){
    if(consumers.count(id)){
        consumers.erase(id);
        return true;
    }
    return false;
}

std::uint64_t Susi::SusiClient::generateId(){
    return std::chrono::system_clock::now().time_since_epoch().count();
}


