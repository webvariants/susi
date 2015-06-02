#include "susi/SusiClient.h"

#include <algorithm>

int main(){

	Susi::SusiClient client{"localhost",4000,"server.key","server.cert"};
	
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