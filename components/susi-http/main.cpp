#include "susi/SusiHTTP.h"

#include <boost/program_options.hpp>
namespace po = boost::program_options;

std::string progName;
std::string addr = "localhost";
unsigned short port = 4000;
std::string keyFile = "server.key";
std::string certFile = "server.cert";
unsigned short httpPort = 4443;

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
		("port,p", po::value<unsigned short>(&port)->default_value(4000), "port of susi instance")
		("key,k", po::value<std::string>(&keyFile)->default_value("http.key"), "keyfile to use")
		("cert,c", po::value<std::string>(&certFile)->default_value("http.cert"), "certificate to use")
		("listen,l", po::value<unsigned short>(&httpPort)->default_value(4443), "port of HTTPS server");
	po::store(po::parse_command_line(argc, argv, desc_), vm_);
	po::notify(vm_);
}


int main(int argc, char *argv[]){
	try{
		parseCommandLine(argc,argv);
		if(vm_.count("help")){
			showHelp();
		}
		SusiHTTP http{addr,port,keyFile,certFile,httpPort};
		http.join();
	}catch(const std::exception & e){
		std::cout << e.what() << std::endl;
		showHelp();
	}
	return 0;
}
