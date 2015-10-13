#include "susi/MQTTClient.h"

#include <boost/program_options.hpp>
namespace po = boost::program_options;

std::string progName;
std::string addr = "localhost";
short port = 4000;
std::string mqttAddr = "localhost";
short mqttPort = 1883;
std::string keyFile = "server.key";
std::string certFile = "server.cert";
std::string dbPath;

std::string forwardTopics = "";
std::string subscribeTopics = "";

std::vector<std::string> forwardTopicsArray;
std::vector<std::string> subscribeTopicsArray;

po::variables_map vm_;
po::options_description desc_{"Allowed options"};

void showHelp(){
	std::cout<<"usage: "<<progName<<std::endl;
	std::cout<<desc_<<std::endl;
	exit(0);
}

void parseCommandLine(int argc, char **argv){
	progName = argv[0];
	desc_.add_options()
		("help,h", "produce help message")
		("addr,a", po::value<std::string>(&addr)->default_value("localhost"), "address of susi instance")
		("port,p", po::value<short>(&port)->default_value(4000), "port of susi instance")
		("mqttAddr,ma", po::value<std::string>(&mqttAddr)->default_value("localhost"), "address of mqtt broker")
		("mqttPort,mp", po::value<short>(&mqttPort)->default_value(1883), "port of mqtt broker")
		("forward,f", po::value<std::string>(&forwardTopics)->default_value(""), "comma seperated list of topics")
		("subscribe,s", po::value<std::string>(&subscribeTopics)->default_value(""), "comma seperated list of topics")
		("key,k", po::value<std::string>(&keyFile)->default_value("leveldb.key"), "keyfile to use")
		("cert,c", po::value<std::string>(&certFile)->default_value("leveldb.cert"), "certificate to use");

	po::store(po::parse_command_line(argc, argv, desc_), vm_);
	po::notify(vm_);
	std::stringstream forwardStream{forwardTopics};
	std::stringstream subscribeStream{subscribeTopics};
	std::string current;
	while(forwardStream.good()){
		std::getline(forwardStream,current,',');
		if(current!=""){
			forwardTopicsArray.push_back(current);
		}
	}
	while(subscribeStream.good()){
		std::getline(subscribeStream,current,',');
		if(current!=""){
			subscribeTopicsArray.push_back(current);
		}
	}
}


int main(int argc, char *argv[]){
	try{
		parseCommandLine(argc,argv);
		if(vm_.count("help")){
			showHelp();
		}
		MQTTClient client{addr,port,keyFile,certFile,mqttAddr,mqttPort};
		for(auto & topic : subscribeTopicsArray){
			client.subscribe(topic);
		}
		for(auto & topic : forwardTopicsArray){
			client.forward(topic);
		}
		client.join();
	}catch(const std::exception & e){
		std::cout << e.what() << std::endl;
		showHelp();
	}
	return 0;
}
