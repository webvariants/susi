#include "apiserver/BasicApiClient.h"

void Susi::Api::BasicApiClient::sendPublish( Susi::Events::Event & event ) {
    Susi::Util::Any packedEvent = event.toAny();
    Susi::Util::Any packet = Susi::Util::Any::Object {
        {"type","publish"},
        {"data",packedEvent}
    };

    JSONTCPClient::send( packet );
}

void Susi::Api::BasicApiClient::sendAck( Susi::Events::Event & event ) {
    Susi::Util::Any packedEvent = event.toAny();
    Susi::Util::Any packet = Susi::Util::Any::Object {
        {"type","ack"},
        {"data",packedEvent}
    };
    JSONTCPClient::send( packet );
}

void Susi::Api::BasicApiClient::sendRegisterConsumer( std::string topic , std::string name) {
    Susi::Util::Any packet = Susi::Util::Any::Object {
        {"type","registerConsumer"},
        {"data",Susi::Util::Any::Object{
            {"topic",topic},
            {"name",name}
        }}
    };
    JSONTCPClient::send( packet );
}

void Susi::Api::BasicApiClient::sendRegisterProcessor( std::string topic , std::string name) {
    Susi::Util::Any packet = Susi::Util::Any::Object {
        {"type","registerProcessor"},
        {"data",Susi::Util::Any::Object{
            {"topic",topic},
            {"name",name}
        }}
    };
    JSONTCPClient::send( packet );
}

void Susi::Api::BasicApiClient::sendShutdown(){
    Susi::Util::Any packet = Susi::Util::Any::Object{
        {"type","shutdown"}
    };
    JSONTCPClient::send( packet );
}

void Susi::Api::BasicApiClient::sendUnregisterConsumer( std::string topic){
    Susi::Util::Any packet = Susi::Util::Any::Object{
        {"type","unregisterConsumer"},
        {"data",Susi::Util::Any::Object{
            {"topic",topic}
        }}
    };
    JSONTCPClient::send( packet );
}
void Susi::Api::BasicApiClient::sendUnregisterProcessor( std::string topic ){
    Susi::Util::Any packet = Susi::Util::Any::Object{
        {"type","unregisterProcessor"},
        {"data",Susi::Util::Any::Object{
            {"topic",topic}
        }}
    };
    JSONTCPClient::send( packet );
}

void Susi::Api::BasicApiClient::onMessage( Susi::Util::Any & message ) {
    std::string type = message["type"];
    if( type=="ack" ) {
        Susi::Events::Event event {message["data"]};
        onAck( event );
    }
    else if( type=="consumerEvent" ) {
        Susi::Events::Event event {message["data"]};
        onConsumerEvent( event );
    }
    else if( type=="processorEvent" ) {
        Susi::Events::Event event {message["data"]};
        onProcessorEvent( event );
    }
    else {
    }
}
