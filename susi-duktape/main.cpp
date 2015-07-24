#include "susi/DuktapeEngine.h"

#include <boost/program_options.hpp>
namespace po = boost::program_options;

std::string progName;
std::string addr = "localhost";
short port = 4000;
std::string keyFile = "server.key";
std::string certFile = "server.cert";
std::string source;

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
        ("key,k", po::value<std::string>(&keyFile)->default_value("key1.pem"), "keyfile to use")
        ("cert,c", po::value<std::string>(&certFile)->default_value("cert1.pem"), "certificate to use")
        ("src,s", po::value<std::string>(&source)->default_value("source.js"), "js source");
    po::store(po::parse_command_line(argc, argv, desc_), vm_);
    po::notify(vm_);
}


int main(int argc, char *argv[]){
	try{
		parseCommandLine(argc,argv);
		if(vm_.count("help")){
			showHelp();
		}
		Susi::SusiClient client{addr,port,keyFile,certFile};
                        Susi::Duktape::JSEngine js{client,source};
                        js.start();
		client.join();
                        std::cout<<"after client join()"<<std::endl;
	}catch(const std::exception & e){
		std::cout << e.what() << std::endl;
		showHelp();
	}
	return 0;
}