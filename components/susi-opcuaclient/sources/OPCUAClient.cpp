#include "susi/OPCUAClient.h"
#include <boost/asio.hpp>
using boost::asio::ip::tcp;

Susi::OPCUAClient::OPCUAClient(Susi::SusiClient & susi) :
  susi{susi} {
    susi.registerProcessor("opcua::client::set",[this](Susi::EventPtr event){
      auto endpoint = event->payload["endpoint"].getString();
      auto node = event->payload["node"].getString();
      auto value = event->payload["value"].getInt64();
      UA_Int32 v = (UA_Int32)value;
      //auto client = createClient("opc.tcp://localhost:16664");
      auto client = createClient(endpoint);
      setInt32(client,node,v);
      this->susi.ack(std::move(event));
    });
    susi.registerProcessor("opcua::client::get",[this](Susi::EventPtr event){
      auto endpoint = event->payload["endpoint"].getString();
      auto node = event->payload["node"].getString();
      //auto client = createClient("opc.tcp://localhost:16664");
      auto client = createClient(endpoint);
      UA_Int32 value = getInt32(client,node);
      event->payload["value"] = (BSON::int64)value;
      this->susi.ack(std::move(event));
    });
    susi.registerProcessor("opcus::client::subscribe",[this](Susi::EventPtr event){
      auto endpoint = event->payload["endpoint"].getString();
      auto node = event->payload["node"].getString();
      auto topic = event->payload["topic"].getString();
      subscribe(endpoint,node,topic);
      this->susi.ack(std::move(event));
    });
}

std::shared_ptr<UA_Client> Susi::OPCUAClient::createClient(std::string endpoint){
  auto deleter = [](UA_Client *ptr){UA_Client_delete(ptr);};
  auto client = std::shared_ptr<UA_Client>{
    UA_Client_new(UA_ClientConfig_standard, Logger_Stdout),
    deleter
  };
  UA_StatusCode retval = UA_Client_connect(client.get(), UA_ClientConnectionTCP,endpoint.c_str());
  if(retval != UA_STATUSCODE_GOOD) {
    throw std::runtime_error{"can not connect to opcua endpoint"};
  }
  return client;
}

UA_Int32 Susi::OPCUAClient::getInt32(std::shared_ptr<UA_Client> client, std::string id){
  UA_Int32 value = 0;
  UA_ReadRequest rReq;
  UA_ReadRequest_init(&rReq);
  rReq.nodesToRead =  (UA_ReadValueId*)UA_Array_new(1, &UA_TYPES[UA_TYPES_READVALUEID]);
  rReq.nodesToReadSize = 1;
  rReq.nodesToRead[0].nodeId = UA_NODEID_STRING_ALLOC(1, (char*)id.c_str()); /* assume this node exists */
  rReq.nodesToRead[0].attributeId = UA_ATTRIBUTEID_VALUE;
  UA_ReadResponse rResp = UA_Client_Service_read(client.get(), rReq);
  if(rResp.responseHeader.serviceResult == UA_STATUSCODE_GOOD &&
     rResp.resultsSize > 0 && rResp.results[0].hasValue &&
     UA_Variant_isScalar(&rResp.results[0].value) &&
     rResp.results[0].value.type == &UA_TYPES[UA_TYPES_INT32]) {
      value = *(UA_Int32*)rResp.results[0].value.data;
  }
  UA_ReadRequest_deleteMembers(&rReq);
  UA_ReadResponse_deleteMembers(&rResp);
  return value;
}

void Susi::OPCUAClient::setInt32(std::shared_ptr<UA_Client> client, std::string id, UA_Int32 value){
  UA_Variant *myVariant = UA_Variant_new();
  UA_Variant_setScalarCopy(myVariant, &value, &UA_TYPES[UA_TYPES_INT32]);
  UA_Client_writeValueAttribute(client.get(), UA_NODEID_STRING(1, (char*)id.c_str()), myVariant);
  UA_Variant_delete(myVariant);
}

void Susi::OPCUAClient::onChange(UA_UInt32 monId, UA_DataValue *value, void *context){
  auto self = (OPCUAClient*)context;
  auto event = self->susi.createEvent(self->subscriptions[monId].topic);
  self->susi.publish(std::move(event));
}

void Susi::OPCUAClient::subscribe(std::string endpoint, std::string nodeId, std::string topic){
  auto client = createClient(endpoint);
  UA_UInt32 subId=0;
  UA_Client_Subscriptions_new(client.get(), UA_SubscriptionSettings_standard, &subId);
  UA_NodeId monitorThis = UA_NODEID_STRING(1, (char*)nodeId.c_str());
  UA_UInt32 monId=0;
  UA_Client_Subscriptions_addMonitoredItem(client.get(), subId, monitorThis,
                                           UA_ATTRIBUTEID_VALUE, &OPCUAClient::onChange, this, &monId);
  subscriptions[monId] = Subscription{
    client,
    topic,
    subId,
    monId
  };
}

void Susi::OPCUAClient::join(){
	susi.join();
}
