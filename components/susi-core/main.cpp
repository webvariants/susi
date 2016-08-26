#include <iostream>
#include "susi/SusiServer.h"

#include <boost/program_options.hpp>
namespace po = boost::program_options;


std::string progName;
short port = 4000;
std::string keyFile = "server.key";
std::string certFile = "server.cert";

po::variables_map vm_;
po::options_description desc_{"Allowed options"};

void showHelp() {
    std::cout << "usage: " << progName << std::endl;
    std::cout << desc_ << std::endl;
    exit(0);
}

void parseCommandLine(int argc, char **argv) {
    progName = argv[0];
    desc_.add_options()
    ("help,h", "produce help message")
    ("port,p", po::value<short>(&port)->default_value(4000), "port to listen")
    ("key,k", po::value<std::string>(&keyFile)->default_value("server.key"), "keyfile to use")
    ("cert,c", po::value<std::string>(&certFile)->default_value("server.cert"), "certificate to use");
    po::store(po::parse_command_line(argc, argv, desc_), vm_);
    po::notify(vm_);
}

int main(int argc, char **argv) {
    try {
        parseCommandLine(argc, argv);
        if (vm_.count("help")) {
            showHelp();
        }
        std::cout << "starting susi server on port " << port << std::endl;
        std::cout << "using ssl certificate " << certFile << std::endl;
        std::cout << "using ssl key " << keyFile << std::endl;
        Susi::SusiServer server{port, keyFile, certFile};
        server.join();
    } catch (const std::exception & e) {
        std::cerr << e.what() << std::endl;
        showHelp();
    }
}
