#include "susi/OPCUA.h"
#include <boost/asio.hpp>
using boost::asio::ip::tcp;

Susi::OPCUA::OPCUA(Susi::SusiClient & susi) :
  susi_{susi} {
    susi_.registerProcessor("opcua::set",[this](Susi::EventPtr event){
      auto endpoint = event->payload["endpoint"].getString();
      auto node = event->payload["node"].getString();
      auto value = event->payload["value"].getInt64();
      UA_Int32 v = (UA_Int32)value;
      //auto client = createClient("opc.tcp://localhost:16664");
      auto client = createClient(endpoint);
      setInt32(client,node,v);
      susi_.ack(std::move(event));
    });
    susi_.registerProcessor("opcua::get",[this](Susi::EventPtr event){
      auto endpoint = event->payload["endpoint"].getString();
      auto node = event->payload["node"].getString();
      //auto client = createClient("opc.tcp://localhost:16664");
      auto client = createClient(endpoint);
      UA_Int32 value = getInt32(client,node);
      event->payload["value"] = (BSON::int64)value;
      susi_.ack(std::move(event));
    });
}

void Susi::OPCUA::join(){
	susi_.join();
}
