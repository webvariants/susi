#include "apiserver/ApiClient.h"

void Susi::Api::ApiClient::publish( Susi::Events::EventPtr event, Susi::Events::Consumer finishCallback, bool processorsOnly, bool consumersOnly) {
    PublishData data{*event,finishCallback};
    publishs[event->getID()] = data;
    sendPublish( *event );
    delete event.release();
}

long Susi::Api::ApiClient::subscribe( std::string topic, Susi::Events::Processor processor , std::string name) {
    sendRegisterProcessor( topic,name );
    subscribes.push_back(SubscribeData{topic,name,true});
    return Susi::Events::Manager::subscribe( topic,processor,name );
}

long Susi::Api::ApiClient::subscribe( std::string topic, Susi::Events::Consumer consumer , std::string name) {
    sendRegisterConsumer( topic,name );
    subscribes.push_back(SubscribeData{topic,name,false});
    return Susi::Events::Manager::subscribe( topic,consumer,name );
}

void Susi::Api::ApiClient::onConsumerEvent( Susi::Events::Event & event ) {
    auto evt = createEvent( event.getTopic() );
    *evt = event;
    Susi::Events::Manager::publish( std::move( evt ), nullptr,false,true);
}

void Susi::Api::ApiClient::onProcessorEvent( Susi::Events::Event & event ) {
    auto evt = createEvent( event.getTopic() );
    *evt = event;
    Susi::Events::Manager::publish( std::move( evt ),[this]( Susi::Events::SharedEventPtr event ) {
        sendAck( *event );
    } ,true, false);
}

void Susi::Api::ApiClient::onAck( Susi::Events::Event & event ) {
    auto evt = std::make_shared<Susi::Events::Event>( event );
    auto publishData = publishs[evt->getID()];
    if( publishData.finishCallback ) {
        publishData.finishCallback( evt );
    }
    publishs.erase(evt->getID());
}

void Susi::Api::ApiClient::onReconnect() {
    LOG(DEBUG) << "we had a reconnect -> resubscribing everything";
    for(auto it : subscribes){
        if(it.processor){
            sendRegisterProcessor(it.topic,it.name);
        }else{
            sendRegisterConsumer(it.topic,it.name);
        }
    }
    for(auto kv : publishs){
        sendPublish(kv.second.event);
    }
}