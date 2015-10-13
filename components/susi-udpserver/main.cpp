#include "susi/SusiUDPServer.h"

#include <boost/program_options.hpp>
#include <fstream>
namespace po = boost::program_options;

std::string progName;
std::string addr = "localhost";
short port = 4000;
std::string keyFile = "server.key";
std::string certFile = "server.cert";
unsigned short udpPort;

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
        ("listen,l", po::value<unsigned short>(&udpPort)->default_value(4001), "udp port where requests are accepted")
        ("key,k", po::value<std::string>(&keyFile)->default_value("udpserver.key"), "keyfile to use")
        ("cert,c", po::value<std::string>(&certFile)->default_value("udpserver.cert"), "certificate to use");
    po::store(po::parse_command_line(argc, argv, desc_), vm_);
    po::notify(vm_);
}

int main(int argc, char *argv[]){
    try{
        parseCommandLine(argc,argv);
        if(vm_.count("help")){
            showHelp();
        }
        Susi::SusiUDPServer server{addr,port,keyFile,certFile,udpPort};
        std::cout<<"started UDP-server on port "<<udpPort<<", start shoveling events to susi ("<<addr<<":"<<port<<")"<<std::endl;
        server.join();
    }catch(const std::exception & e){
        std::cout << e.what() << std::endl;
        showHelp();
    }
    return 0;
}