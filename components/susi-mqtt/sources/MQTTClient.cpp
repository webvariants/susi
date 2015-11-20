#include "susi/MQTTClient.h"

Susi::MQTTClient::MQTTClient(Susi::SusiClient & susi, BSON::Value & config) :
  susi_{susi} {
      auto addr = config["mqtt-addr"].getString();
      auto port = static_cast<short>(config["mqtt-port"].getInt64());
      int keepAlive = 60;
      mosqpp::mosquittopp::connect(addr.c_str(),port,keepAlive);
      runloop_ = std::move(std::thread{[this](){
        while(true){
            int rc = mosqpp::mosquittopp::loop();
    		if(rc){
    			mosqpp::mosquittopp::reconnect();
    		};
        }
      }});
      auto forwardTopics = config["forward"].getArray();
      for(auto & topic : forwardTopics){
          forward(topic);
      }
      auto subscribeTopics = config["subscribe"].getArray();
      for(auto & topic : subscribeTopics){
          subscribe(topic);
      }
}

void Susi::MQTTClient::on_connect(int rc) {
	printf("Connected with code %d.\n", rc);
    for(auto & topic : subscriptions_){
        mosqpp::mosquittopp::subscribe(NULL,topic.c_str());
    }
}

void Susi::MQTTClient::on_message(const struct mosquitto_message *message) {
    std::string msgString((char*)message->payload,message->payloadlen);
    auto event = susi_.createEvent(message->topic);
    event->payload = msgString;
    susi_.publish(std::move(event));
}

void Susi::MQTTClient::on_subscribe(int mid, int qos_count, const int *granted_qos) {
	printf("Subscription succeeded.\n");
}

void Susi::MQTTClient::subscribe(const std::string & topic) {
    subscriptions_.push_back(topic);
    mosqpp::mosquittopp::subscribe(NULL,topic.c_str());
    std::cout<<"subscribe to topic "<<topic<<std::endl;
}

void Susi::MQTTClient::forward(const std::string & topic) {
    std::cout<<"setup forwarding for topic "<<topic<<std::endl;
    susi_.registerConsumer(topic,[this](Susi::SharedEventPtr event){
        std::string msg = event->toString();
        mosqpp::mosquittopp::publish(NULL, event->topic.c_str(), msg.size(), msg.c_str());
    });
}

void Susi::MQTTClient::join(){
	susi_.join();
}
