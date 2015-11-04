#include "susi/SusiStateFileClient.h"

#include <boost/program_options.hpp>
namespace po = boost::program_options;

std::string progName;
std::string addr = "localhost";
short port = 4000;
std::string keyFile = "server.key";
std::string certFile = "server.cert";
std::string stateFile;

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
		("file,f", po::value<std::string>(&stateFile)->default_value("/tmp/susi-states.json"), "where to store the state")
		("key,k", po::value<std::string>(&keyFile)->default_value("leveldb.key"), "keyfile to use")
		("cert,c", po::value<std::string>(&certFile)->default_value("leveldb.cert"), "certificate to use");
	po::store(po::parse_command_line(argc, argv, desc_), vm_);
	po::notify(vm_);
}


int main(int argc, char *argv[]){
	try{
		parseCommandLine(argc,argv);
		if(vm_.count("help")){
			showHelp();
		}
		SusiStateFileClient client{addr,port,keyFile,certFile,stateFile};
		std::cout<<"registered susi-statefile processors at susi ("<<addr<<":"<<port<<")."<<std::endl;
		client.join();
	}catch(const std::exception & e){
		std::cout << e.what() << std::endl;
		showHelp();
	}
	return 0;
}
