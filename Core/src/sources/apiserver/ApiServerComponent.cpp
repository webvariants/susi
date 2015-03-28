#include "susi/apiserver/ApiServerComponent.h"

void Susi::Api::ApiServerComponent::onConnect( std::string & id ) {
    LOG(DEBUG) << "new api connection: " << id;
    sessionManager->updateSession( id );
}

void Susi::Api::ApiServerComponent::onClose( std::string & id ) {
    LOG(DEBUG) << "lost api connection: " << id;
    sessionManager->killSession( id );
    {
        std::lock_guard<std::recursive_mutex> lock{consumerMutex};
        auto & subs = consumerSubscriptions[id];
        for( auto & kv : subs ) {
            eventManager->unsubscribe( kv.second );
        }
        consumerSubscriptions.erase( id );
    }
    {
        std::lock_guard<std::recursive_mutex> lock{processorMutex};
        auto & subs2 = processorSubscriptions[id];
        for( auto & kv : subs2 ) {
            eventManager->unsubscribe( kv.second );
        }
        processorSubscriptions.erase( id );
    }
    {
        std::lock_guard<std::recursive_mutex> lock{eventsMutex};
        eventsToAck.erase( id );
    }
    {
        std::lock_guard<std::recursive_mutex> lock{sendersMutex};
        senders.erase( id );
    }
}

void Susi::Api::ApiServerComponent::onMessage( std::string & id, BSON::Value & packet ) {
    LOG(DEBUG) << "got message in apiserver: "<<packet.toJSON();
    try {
        BSON::Value & type = packet["type"];
        if( type.isString() ) {
            std::string typeStr = type;
            if( typeStr == "registerConsumer" ) {
                handleRegisterConsumer( id,packet );
            }
            else if( typeStr == "registerProcessor" ) {
                handleRegisterProcessor( id,packet );
            }
            else if( typeStr == "unregisterConsumer" ) {
                handleUnregisterConsumer( id,packet );
            }
            else if( typeStr == "unregisterProcessor" ) {
                handleUnregisterProcessor( id,packet );
            }
            else if( typeStr == "publish" ) {
                handlePublish( id,packet );
            }
            else if( typeStr == "ack" ) {
                handleAck( id,packet );
            }
            else {
                sendFail( id,"type not known" );
            }
        }
        else {
            sendFail( id,"type is not a string" );
        }
    }
    catch( const std::exception & e ) {
        std::string msg = "exception while processing: ";
        msg += e.what();
        LOG(ERROR) << msg;
        sendFail( id,msg );
    }
}

void Susi::Api::ApiServerComponent::handleRegisterConsumer( std::string & id, BSON::Value & packet ) {
    auto & data = packet["data"];
    if( data.isObject() ) {
        std::string topic = data["topic"];
        std::string subName = "";
        if(data["name"].isString()){
            subName = data["name"].getString();
        }
        std::lock_guard<std::recursive_mutex> lock{consumerMutex};
        auto & subs = consumerSubscriptions[id];
        if( subs.find( topic ) != subs.end() ) {
            LOG(DEBUG) << "failed to registerConsumer for "<<id<<" to topic "<<topic<<": allready subscribed";
            sendFail( id,"you are allready subscribed to "+topic );
            return;
        }
        Susi::Events::Consumer callback = [this,id]( Susi::Events::SharedEventPtr event ) {
            BSON::Value packet;
            packet["type"] = "consumerEvent";
            packet["data"] = event->toAny();
            if(!checkIfConfidentialHeaderMatchesSession(*event,id)){
                return;
            }
            std::string _id = id;
            send( _id,packet );
        };
        long subid = eventManager->subscribe( topic,callback,subName );
        LOG(DEBUG) << "registerConsumer for "<<id<<" to topic "<<topic;
        subs[topic] = subid;
        LOG(DEBUG) << "register consumer for topic "<<topic<<" to session "<<id;
        sendOk( id );
    }
    else {
        sendFail( id,"data is not a object" );
    }
}

void Susi::Api::ApiServerComponent::handleRegisterProcessor( std::string & id, BSON::Value & packet ) {
    auto & data = packet["data"];
    if( data.isObject() ) {
        std::string topic = data["topic"];
        std::string subName = "";
        if(data["name"].isString()){
            subName = data["name"].getString();
        }
        std::lock_guard<std::recursive_mutex> lock{processorMutex};
        auto & subs = processorSubscriptions[id];
        if( subs.find( topic ) != subs.end() ) {
            LOG(DEBUG) << "failed to registerProcessor for "<<id<<" to topic "<<topic<<": allready subscribed";
            sendFail( id,"you are allready subscribed to "+topic );
            return;
        }
        long subid = eventManager->subscribe( topic,Susi::Events::Processor{[this,id]( Susi::Events::EventPtr event ) {
            BSON::Value packet;
            packet["type"] = "processorEvent";
            packet["data"] = event->toAny();
            if(!checkIfConfidentialHeaderMatchesSession(*event,id)){
                return;
            }
            std::string _id = id;
            {
                std::lock_guard<std::recursive_mutex> eventLock{eventsMutex};
                eventsToAck[_id][event->id] = std::move( event );
            }
            send( _id,packet );
        }},subName );
        subs[topic] = subid;
        LOG(DEBUG) << "registerProcessor for "<<id<<" to topic "<<topic;
        sendOk( id );
    }
    else {
        sendFail( id,"data is not a object" );
    }
}
void Susi::Api::ApiServerComponent::handleUnregisterConsumer( std::string & id, BSON::Value & packet ) {
    auto & data = packet["data"];
    if( data.isObject() ) {
        std::string topic = data["topic"];
        std::lock_guard<std::recursive_mutex> lock{consumerMutex};
        auto & subs = consumerSubscriptions[id];
        if( subs.find( topic )!=subs.end() ) {
            long subid = subs[topic];
            eventManager->unsubscribe( subid );
            LOG(DEBUG) << "unregister consumer for topic "<<topic<<" from session "<<id;
            subs.erase(topic);
            sendOk( id );
            LOG(DEBUG) << "unregisterConsumer for "<<id<<" from "<<topic;
        }
        else {
            LOG(DEBUG) << "unregisterConsumer for "<<id<<" from "<<topic<<" failed: id not subscribed";
            sendFail( id,"you are not registered for "+topic );
        }
    }
    else {
        sendFail( id,"data is not a object" );
    }
}

void Susi::Api::ApiServerComponent::handleUnregisterProcessor( std::string & id, BSON::Value & packet ) {
    auto & data = packet["data"];
    if( data.isObject() ) {
        std::string topic = data["topic"];
        std::lock_guard<std::recursive_mutex> lock{processorMutex};
        auto & subs = processorSubscriptions[id];
        if( subs.find( topic )!=subs.end() ) {
            long subid = subs[topic];
            eventManager->unsubscribe( subid );
            LOG(DEBUG) << "unregister processor for topic "<<topic<<" from session "<<id;
            sendOk( id );
            LOG(DEBUG) << "unregisterConsumer for "<<id<<" from "<<topic;
        }
        else {
            LOG(DEBUG) << "unregisterConsumer for "<<id<<" from "<<topic<<" failed: id not subscribed";
            sendFail( id,"you are not registered for "+topic );
        }
    }
    else {
        sendFail( id,"data is not a object" );
    }
}
void Susi::Api::ApiServerComponent::handlePublish( std::string & id, BSON::Value & packet ) {
    auto & eventData = packet["data"];
    if( !eventData.isObject() || !eventData["topic"].isString() ) {
        sendFail( id,"publish handler: data is not an object or topic is not set correctly" );
        return;
    }

    auto event = eventManager->createEvent( eventData["topic"] );
    Susi::Events::Event rawEvent {eventData};
    rawEvent.sessionID = id;
    if( rawEvent.id == "" ) {
        rawEvent.id = std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    }
    *event = rawEvent;
    
    LOG(DEBUG) << "publish event from "<<id<<", topic: "<<event->topic;
    eventManager->publish( std::move( event ),[this,id]( Susi::Events::SharedEventPtr event ) {
        LOG(DEBUG) << "publish ready, in finish callback";
        BSON::Value packet;
        packet["type"] = "ack";
        packet["data"] = event->toAny();
        std::string _id = id;
        send( _id,packet );
        LOG(DEBUG) << "after send back of finishcallback, id: "<<_id;
    } );

    sendOk( id );
}

void Susi::Api::ApiServerComponent::handleAck( std::string & id, BSON::Value & packet ) {
    auto & eventData = packet["data"];
    if( !eventData.isObject() || !eventData["topic"].isString() ) {
        sendFail( id,"ack handler: data is not an object or topic is not set correctly" );
        return;
    }
    std::string eventID = eventData["id"];
    std::lock_guard<std::recursive_mutex> lock{eventsMutex};
    if(!(eventsToAck.count(id)>0) || !(eventsToAck[id].count(eventID)>0)){
        LOG(DEBUG) << "unexpected ack from "<<id<<" for event with topic "<<eventData["id"].getString();
        sendFail( id , "unexpected ack" );
        return;
    }
    auto event = std::move( eventsToAck[id][eventID] );
    eventsToAck[id].erase( eventID );
    event->headers.clear();
    event->id = eventID;
    if( eventData["headers"].isArray() ) {
        BSON::Array arr = eventData["headers"];
        for( BSON::Object & val : arr ) {
            for( auto & kv : val ) {
                event->headers.push_back( std::make_pair( kv.first,kv.second.getString() ) );
            }
        }
    }
    if( !eventData["payload"].isUndefined() ) {
        event->payload = eventData["payload"];
    }
    LOG(DEBUG) << "got ack from "<<id<<" for event with topic "<<event->topic;
    eventManager->ack( std::move( event ) );
    sendOk( id );
}

void Susi::Api::ApiServerComponent::sendOk( std::string & id ) {
    /* DO NOT SEND "OK" STATUS MESSAGES ANYMORE!
    BSON::Value response;
    response["type"] = "status";
    response["error"] = false;
    send( id,response );
    */
}

void Susi::Api::ApiServerComponent::sendFail( std::string & id,std::string error ) {
    BSON::Value response;
    response["type"] = "error";
    response["data"] = error;
    send( id,response );
}


bool Susi::Api::ApiServerComponent::checkIfConfidentialHeaderMatchesSession(Susi::Events::Event & event, std::string sessionID){
    long sessionAuthlevel = sessionManager->getSessionAttribute(sessionID,"authlevel").getInt32();
    for(auto & pair : event.headers){
        if(pair.first == "confidential"){
            long eventAuthlevel = std::stol(pair.second);
            if(eventAuthlevel<sessionAuthlevel){
                return false;
            }
        }
    }
    return true;
}