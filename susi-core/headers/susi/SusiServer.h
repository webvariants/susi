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

#ifndef __SUSISERVER__
#define __SUSISERVER__

#include "susi/FramingServer.h"
#include "susi/JSONFramer.h"
#include <regex>
#include <chrono>
#include <bson/Value.h>

#ifdef WITH_SSL
    #include "susi/TCPServer.h"
    #include "susi/SSLTCPServer.h"
#else
    #include "susi/TCPServer.h"
#endif

namespace Susi {
    
    template<class BaseServer>   
    class SusiServer : public FramingServer<JSONFramer,BaseServer> {
    protected:
        std::map<std::string, std::vector<int>> consumers;  //topic to list of client id's
        std::map<std::string, std::vector<int>> processors; //topic to list of client id's

        struct PublishProcess {
            BSON::Value lastState;
            std::vector<int> processors;    //list of client id's
            std::vector<int> consumers;     //list of client id's
            size_t nextProcessor = 0;
            int publisher;                  //client id of the publisher
        };

        std::map<std::string,std::shared_ptr<PublishProcess>> publishProcesses; //event id to publish process

    public:
        SusiServer(short port) : FramingServer<JSONFramer,BaseServer>{port} {}
        #ifdef WITH_SSL
            SusiServer(short port, std::string keyFile, std::string certificateFile) :
                FramingServer<JSONFramer,BaseServer>{port,keyFile,certificateFile} {}
        #endif
        
        
        virtual ~SusiServer() {}

        virtual void onConnect(int id) override {
            std::cout<<"got new client "<<BaseServer::getPeerCertificateHash(id)<<std::endl;
            std::cout<<BaseServer::getPeerCertificate(id)<<std::endl;
            FramingServer<JSONFramer,BaseServer>::onConnect(id);
            BSON::Value sessionNewEvent = BSON::Object{
                {"topic","core::session::new"},
                {"payload", id}
            };
            publish(sessionNewEvent,0);
        }

        virtual void onClose(int id) override {
            std::cout<<"lost client "<<id<<std::endl;
            FramingServer<JSONFramer,BaseServer>::onClose(id);

            //remove all publish processes associated with this client
            for (auto it = publishProcesses.cbegin(); it != publishProcesses.cend();){
              if (it->second->publisher == id){
                publishProcesses.erase(it++);
              } else {
                ++it;
              }
            }

            //remove all associated consumers
            for(auto & kv : consumers){
                kv.second.erase(std::remove_if(kv.second.begin(), kv.second.end(), 
                   [id](int currentId) { return currentId == id; }), kv.second.end());
            }
            
            //remove all associated processors
            for(auto & kv : processors){
                kv.second.erase(std::remove_if(kv.second.begin(), kv.second.end(), 
                   [id](int currentId) { return currentId == id; }), kv.second.end());
            }

            //cleanup publishProcesses
            std::vector<std::shared_ptr<PublishProcess>> orphanedProcesses;
            for(auto & kv : publishProcesses) {
                auto process = kv.second;
                std::cout<<"process: "<<process<<std::endl;
                // check if there are pending processorEvents for this client
                if(process->nextProcessor-1 < process->processors.size() && process->processors[process->nextProcessor-1] == id){
                    orphanedProcesses.push_back(process);
                }
                //remove all id occurences from the process
                for(size_t i=process->nextProcessor;i<process->processors.size();i++){
                    if(process->processors[i] == id){
                        process->processors.erase(process->processors.begin()+i);
                        break;
                    }
                }
                for(size_t i=0;i<process->consumers.size();i++){
                    if(process->consumers[i] == id){
                        process->consumers.erase(process->consumers.begin()+i);
                        break;
                    }
                }
            }
            for(auto & p : orphanedProcesses){
                ack(p->lastState);
            }
            BSON::Value sessionLostEvent = BSON::Object{
                {"topic","core::session::lost"},
                {"payload", id}
            };
            publish(sessionLostEvent,0);
        }

        virtual void onFrame(int id, std::string & frame) override {
            auto doc = BSON::Value::fromJSON(frame);
            if(!validateFrame(doc)){
                BSON::Value res = BSON::Object{
                    {"type","error"},
                    {"data",BSON::Object{
                        {"reason","your request is invalid"}
                    }}
                };
                send(id, res);
            }else{
                std::string & type = doc["type"];
                BSON::Value & data = doc["data"];
                if(type == "registerConsumer"){
                    registerConsumer(data["topic"],id);
                }
                if(type == "registerProcessor"){
                    registerProcessor(data["topic"],id);
                }
                if(type == "unregisterConsumer"){
                    unregisterConsumer(data["topic"],id);
                }
                if(type == "unregisterProcessor"){
                    unregisterProcessor(data["topic"],id);
                }
                if(type == "publish"){
                    publish(data,id);
                }
                if(type == "ack"){
                    ack(data);
                }
            }
        }

    protected:
        bool validateFrame(BSON::Value & doc){
            if( !doc.isObject() || 
                !doc["type"].isString() || 
                !doc["data"].isObject() ||
                !doc["data"]["topic"].isString()){
                return false;
            }
            return true;
        }

        void send(int id, BSON::Value & doc){
            std::string frame = doc.toJSON()+"\n";
            FramingServer<JSONFramer,BaseServer>::send(id, frame.c_str(), frame.size());
        }

        void registerConsumer(std::string & topic, int id){
            std::cout<<"register consumer for "<<id<<std::endl;
            if(!contains(consumers[topic],id)){
                consumers[topic].push_back(id);
            }
        }

        void registerProcessor(std::string & topic, int id){
            std::cout<<"register processor for "<<id<<std::endl;
            if(!contains(processors[topic],id)){
                processors[topic].push_back(id);
            }
        }

        void unregisterConsumer(std::string & topic, int id){
            for(int i=0;i<consumers[topic].size();i++){
                if(id == consumers[topic][i]){
                    consumers[topic].erase(consumers[topic].begin()+i);
                    break;
                }
            }
        }

        void unregisterProcessor(std::string & topic, int id){
            for(int i=0;i<processors[topic].size();i++){
                if(id == processors[topic][i]){
                    processors[topic].erase(processors[topic].begin()+i);
                    break;
                }
            }
        }

        void publish(BSON::Value & event, int publisher){
            std::cout<<"publish event for "<<publisher<<std::endl;
            std::string & topic = event["topic"];
            if(!event["id"].isString()){
                long id = std::chrono::system_clock::now().time_since_epoch().count();
                event["id"] = std::to_string(id);
            }
            if(!event["sessionid"].isString()){
                event["sessionid"] = std::to_string(publisher);
            }

            auto peerCertHash = BaseServer::getPeerCertificateHash(publisher);
            if(peerCertHash!=""){
                if(event["headers"].isArray()){
                    event["headers"].push_back(BSON::Object{{"certHash",peerCertHash}});
                }else{
                    event["headers"] = BSON::Array{BSON::Object{{"certHash",peerCertHash}}};
                }
            }

            checkAndReactToSusiEvents(event);

            auto process = std::make_shared<PublishProcess>();
            process->publisher = publisher;
            for(auto & kv : processors){
                std::regex e{kv.first};
                if(std::regex_match(topic,e)){
                    for(auto & id : kv.second){
                        if(!contains(process->processors,id)){
                            process->processors.push_back(id);
                        }
                    }
                }
            }
            for(auto & kv : consumers){
                std::regex e{kv.first};
                if(std::regex_match(topic,e)){
                    for(auto & id : kv.second){
                        if(!contains(process->consumers,id)){
                            process->consumers.push_back(id);
                        }
                    }
                }
            }
            publishProcesses[event["id"].getString()] = process;
            ack(event);
        }

        void ack(BSON::Value & event){
            std::string & id = event["id"];
            if(publishProcesses.count(id)==0){
                return;
            }
            auto process = publishProcesses[id];
            process->lastState = event;
            if(process->nextProcessor >= process->processors.size()){
                //processor phase finished, send to all consumers and to publisher
                BSON::Value consumerEvent = BSON::Object{
                    {"type","consumerEvent"},
                    {"data", event}
                };
                for(auto & id : process->consumers){
                    send(id,consumerEvent);
                }
                BSON::Value publisherAck = BSON::Object{
                    {"type","ack"},
                    {"data", event}
                };
                std::cout<<"send ack to publisher "<<process->publisher<<std::endl;
                send(process->publisher,publisherAck);
                publishProcesses.erase(id);
            }else{
                std::cout<<"forward event to "<<process->processors[process->nextProcessor]<<std::endl;
                BSON::Value processorEvent = BSON::Object{
                    {"type","processorEvent"},
                    {"data", event}
                };
                send(process->processors[process->nextProcessor++], processorEvent);
            }
        }

        bool contains(std::vector<int> & vec, int elem){
            for(auto & id : vec){
                if(id == elem){
                    return true;
                }
            }
            return false;
        }

        void checkAndReactToSusiEvents(BSON::Value & event){
            std::regex susiEventTopic{"core::.*"};
            if(std::regex_match(event["topic"].getString(),susiEventTopic)){
                if(event["topic"].getString() == "core::session::getCertificate"){
                    long long id = event["payload"].getInt64();
                    std::string cert = BaseServer::getPeerCertificate(id);
                    event["payload"] = cert;
                }
            }
        }

    };


typedef SusiServer<TCPServer> SmallSusiServer;
#ifdef WITH_SSL
    typedef SusiServer<SSLTCPServer> SecureSusiServer;
#endif

}

#endif // __SUSISERVER__