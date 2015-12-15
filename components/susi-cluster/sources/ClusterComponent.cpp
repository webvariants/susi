#include "susi/ClusterComponent.h"

Susi::ClusterComponent::ClusterComponent(Susi::SusiClient & susi, BSON::Value & config) :
  _susi{susi},
  _config{config} {
      std::cout<<"cluster config: "<<config.toJSON()<<std::endl;
      if(_config["nodes"].isArray()){
          for(std::size_t nodeId=0;nodeId < _config["nodes"].size(); nodeId++){
              auto & node = _config["nodes"][nodeId];
              registerNode(node["id"],node["addr"],static_cast<short>(node["port"].getInt64()),node["key"],node["cert"]);
              if(node["forwardProcessors"].isArray()){
                  for(std::size_t i=0;i< node["forwardProcessors"].size();i++){
                      std::string & topic = node["forwardProcessors"][i];
                      forwardProcessorEvent(topic,node["id"]);
                      std::cout<<"setup processor forwarding of "<<topic<<" to "<<node["id"].getString()<<std::endl;
                  }
              }
              if(node["forwardConsumers"].isArray()){
                  for(std::size_t i=0;i< node["forwardConsumers"].size();i++){
                      std::string & topic = node["forwardConsumers"][i];
                      forwardConsumerEvent(topic,node["id"]);
                      std::cout<<"setup consumer forwarding of "<<topic<<" to "<<node["id"].getString()<<std::endl;
                  }
              }
              if(node["registerConsumers"].isArray()){
                  for(std::size_t i=0;i< node["registerConsumers"].size();i++){
                      std::string & topic = node["registerConsumers"][i];
                      registerConsumer(topic,node["id"]);
                      std::cout<<"register consumer for "<<topic<<" at "<<node["id"].getString()<<std::endl;
                  }
              }
              if(node["registerProcessors"].isArray()){
                  for(std::size_t i=0;i< node["registerProcessors"].size();i++){
                      std::string & topic = node["registerProcessors"][i];
                      registerProcessor(topic,node["id"]);
                      std::cout<<"register processor for "<<topic<<" at "<<node["id"].getString()<<std::endl;
                  }
              }
          }
      }
}

void Susi::ClusterComponent::join(){
    _susi.join();
}

void Susi::ClusterComponent::registerNode(std::string id, std::string addr,short port, std::string key, std::string cert){
    _nodes[id] = std::make_shared<Susi::SusiClient>(addr,port,key,cert);
}

bool Susi::ClusterComponent::forwardProcessorEvent(std::string topic, std::string id){
    if(_nodes.count(id)==0){
        return false;
    }
    auto & node = _nodes[id];
    _susi.registerProcessor(topic,[node,this,id](Susi::EventPtr event){
        std::cout<<"forward processor "<<event->topic<<" to "<<id<<std::endl;
        if(!node->connected()){
            event->headers.push_back({"Error","node "+id+" not connected"});
            return;
        }
        struct FinishCallback {
            Susi::EventPtr localEvent;
            std::string id;
            FinishCallback(Susi::EventPtr evt) :
                localEvent{std::move(evt)} {}
            FinishCallback(FinishCallback && other) :
                localEvent{std::move(other.localEvent)} {}
            FinishCallback(FinishCallback & other) :
                localEvent{std::move(other.localEvent)} {}
            void operator()(Susi::SharedEventPtr remoteEvent){
                *localEvent = *remoteEvent;
            }
        };
        auto remoteEvent = node->createEvent(event->topic);
        *remoteEvent = *event;
        node->publish(std::move(remoteEvent),FinishCallback{std::move(event)});
    });
    return true;
}

bool Susi::ClusterComponent::forwardConsumerEvent(std::string topic, std::string id){
    if(_nodes.count(id)==0){
        return false;
    }
    auto & node = _nodes[id];
    _susi.registerConsumer(topic,[node,id](Susi::SharedEventPtr event){
        std::cout<<"forward consumer "<<event->topic<<" to "<<id<<std::endl;
        auto remoteEvent = node->createEvent(event->topic);
        *remoteEvent = *event;
        std::cout<<"forward consumer event to node "<<id<<std::endl;
        node->publish(std::move(remoteEvent),[](Susi::SharedEventPtr event){
            std::cout<<"got finishcallback from forwared consumer event"<<std::endl;
        });
    });
    return true;
}

bool Susi::ClusterComponent::registerProcessor(std::string topic, std::string id){
    if(_nodes.count(id)==0){
        return false;
    }
    auto & node = _nodes[id];
    node->registerProcessor(topic,[this,topic,id](Susi::EventPtr remoteEvent){
        std::cout<<"got processor event "<<topic<<" from "<<id<<std::endl;
        struct FinishCallback {
            Susi::EventPtr remoteEvent;
            FinishCallback(Susi::EventPtr evt) :
                remoteEvent{std::move(evt)} {}
            FinishCallback(FinishCallback && other) :
                remoteEvent{std::move(other.remoteEvent)} {}
            FinishCallback(FinishCallback & other) :
                remoteEvent{std::move(other.remoteEvent)} {}
            void operator()(Susi::SharedEventPtr localEvent){
                std::cout<<"send ack for event "<<localEvent->topic<<" back to origin"<<std::endl;
                *remoteEvent = *localEvent;
            }
        };
        auto localEvent = _susi.createEvent(remoteEvent->topic);
        *localEvent = *remoteEvent;
        FinishCallback cb{std::move(remoteEvent)};
        _susi.publish(std::move(localEvent),std::move(cb));
    });
    return true;
}

bool Susi::ClusterComponent::registerConsumer(std::string topic, std::string id){
    if(_nodes.count(id)==0){
        return false;
    }
    auto & node = _nodes[id];
    node->registerConsumer(topic,[this,topic,id](Susi::SharedEventPtr remoteEvent){
        std::cout<<"got consumer event "<<topic<<" from "<<id<<std::endl;
        auto localEvent = _susi.createEvent(remoteEvent->topic);
        *localEvent = *remoteEvent;
        _susi.publish(std::move(localEvent));
    });
    return true;
}
