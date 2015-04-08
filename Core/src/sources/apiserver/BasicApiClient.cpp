#include "susi/apiserver/BasicApiClient.h"

void Susi::Api::BasicApiClient::sendPublish( Susi::Events::Event & event ) {
    BSON::Value packedEvent = event.toAny();
    BSON::Value packet = BSON::Object {
        {"type","publish"},
        {"data",packedEvent}
    };

    JSONTCPClient::send( packet );
}

void Susi::Api::BasicApiClient::sendAck( Susi::Events::Event & event ) {
    BSON::Value packedEvent = event.toAny();
    BSON::Value packet = BSON::Object {
        {"type","ack"},
        {"data",packedEvent}
    };
    JSONTCPClient::send( packet );
}

void Susi::Api::BasicApiClient::sendRegisterConsumer( std::string topic , std::string name) {
    BSON::Value packet = BSON::Object {
        {"type","registerConsumer"},
        {"data",BSON::Object{
            {"topic",topic},
            {"name",name}
        }}
    };
    JSONTCPClient::send( packet );
}

void Susi::Api::BasicApiClient::sendRegisterProcessor( std::string topic , std::string name) {
    BSON::Value packet = BSON::Object {
        {"type","registerProcessor"},
        {"data",BSON::Object{
            {"topic",topic},
            {"name",name}
        }}
    };
    JSONTCPClient::send( packet );
}

void Susi::Api::BasicApiClient::sendShutdown(){
    BSON::Value packet = BSON::Object{
        {"type","shutdown"}
    };
    JSONTCPClient::send( packet );
}

void Susi::Api::BasicApiClient::sendUnregisterConsumer( std::string topic){
    BSON::Value packet = BSON::Object{
        {"type","unregisterConsumer"},
        {"data",BSON::Object{
            {"topic",topic}
        }}
    };
    JSONTCPClient::send( packet );
}
void Susi::Api::BasicApiClient::sendUnregisterProcessor( std::string topic ){
    BSON::Value packet = BSON::Object{
        {"type","unregisterProcessor"},
        {"data",BSON::Object{
            {"topic",topic}
        }}
    };
    JSONTCPClient::send( packet );
}

void Susi::Api::BasicApiClient::onMessage( BSON::Value & message ) {
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
