#include "susi/MQTTClient.h"

MQTTClient::MQTTClient(std::string addr,short port, std::string key, std::string cert, std::string mqttAddr, short mqttPort) :
  susi_{new Susi::SusiClient{addr,port,key,cert}} {
      int keepAlive = 60;
      mosqpp::mosquittopp::connect(mqttAddr.c_str(),mqttPort,keepAlive);
      runloop_ = std::move(std::thread{[this](){
        while(true){
            int rc = mosqpp::mosquittopp::loop();
    		if(rc){
    			mosqpp::mosquittopp::reconnect();
    		};
        }
      }});
}

void MQTTClient::on_connect(int rc) {
	printf("Connected with code %d.\n", rc);
    for(auto & topic : subscriptions_){
        mosqpp::mosquittopp::subscribe(NULL,topic.c_str());
    }
}

void MQTTClient::on_message(const struct mosquitto_message *message) {
    std::string msgString((char*)message->payload,message->payloadlen);
    auto event = susi_->createEvent(message->topic);
    event->payload = msgString;
    susi_->publish(std::move(event));
}

void MQTTClient::on_subscribe(int mid, int qos_count, const int *granted_qos) {
	printf("Subscription succeeded.\n");
}

void MQTTClient::subscribe(const std::string & topic) {
    subscriptions_.push_back(topic);
    mosqpp::mosquittopp::subscribe(NULL,topic.c_str());
    std::cout<<"subscribe to topic "<<topic<<std::endl;
}

void MQTTClient::forward(const std::string & topic) {
    std::cout<<"setup forwarding for topic "<<topic<<std::endl;
    susi_->registerConsumer(topic,[this](Susi::SharedEventPtr event){
        std::string msg = event->toString();
        mosqpp::mosquittopp::publish(NULL, event->topic.c_str(), msg.size(), msg.c_str());
    });
}

void MQTTClient::join(){
	susi_->join();
}
