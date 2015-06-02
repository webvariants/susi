#include "susi/SusiClient.h"
#include <iostream>

int main(){
	Susi::SusiClient client{"localhost",4000,"server.key","server.cert"};

	std::cout<<"My certificate: "<<std::endl;
	std::cout<<client.getCertificate()<<std::endl;
	std::cout<<"My ID: "<<std::endl;
	std::cout<<client.getCertificateHash()<<std::endl;


	client.registerProcessor("foobar",[](BSON::Value & event){
		event["payload"] = BSON::Array{};
		event["payload"].push_back("fuck");
	});


	client.registerProcessor("foobar",[](BSON::Value & event){
		if(event["payload"].isArray()){
			event["payload"].push_back("yeah");
		}
	});

	client.registerConsumer("foobar",[](const BSON::Value & event){
		std::cout<<"consumer: "<<event.toJSON()<<std::endl;
	});

	client.publish("foobar",BSON::Value{},[](const BSON::Value & event){
		std::cout<<"ack: "<<event.toJSON()<<std::endl;
	});

	client.join();
	return 0;
}