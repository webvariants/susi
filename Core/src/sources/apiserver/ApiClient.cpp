#include "apiserver/ApiClient.h"

void Susi::Api::ApiClient::publish( Susi::Events::EventPtr event, Susi::Events::Consumer finishCallback ) {
    sendPublish( *event );
    publishData[event->getID()] = PublishData{std::move(event),finishCallback};
}

long Susi::Api::ApiClient::subscribe( std::string topic, Susi::Events::Processor processor , std::string name) {
    sendRegisterProcessor( topic,name );
    long id = Susi::Events::Manager::subscribe( topic,processor,name );
    subscribes[id] = SubscribeData{topic,name,true};
    return id;
}

long Susi::Api::ApiClient::subscribe( std::string topic, Susi::Events::Consumer consumer , std::string name) {
    sendRegisterConsumer( topic,name );
    long id = Susi::Events::Manager::subscribe( topic,consumer,name );
    subscribes[id] = SubscribeData{topic,name,false};
    return id;
}

void Susi::Api::ApiClient::onConsumerEvent( Susi::Events::Event & event ) {
    auto evt = createEvent( event.getTopic() );
    *evt = event;
    Susi::Events::Manager::publish( std::move( evt ) );
}

void Susi::Api::ApiClient::onProcessorEvent( Susi::Events::Event & event ) {
    auto evt = createEvent( event.getTopic() );
    *evt = event;
    Susi::Events::Manager::publish( std::move( evt ),[this]( Susi::Events::SharedEventPtr event ) {
        sendAck( *event );
    } );
}

void Susi::Api::ApiClient::onAck( Susi::Events::Event & event ) {
    auto evt = std::make_shared<Susi::Events::Event>( event );
    auto & data = publishData[evt->getID()];
    delete data.event.release();
    if( data.finishCallback ) {
        data.finishCallback( evt );
    }
    publishData.erase(evt->getID());
}