#include "susi/SusiShell.h"

#include <boost/program_options.hpp>
#include <fstream>
namespace po = boost::program_options;

std::string progName;
std::string config;

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
        ("conf,c", po::value<std::string>(&config)->default_value("shellconfig.json"), "address of susi instance");
    po::store(po::parse_command_line(argc, argv, desc_), vm_);
    po::notify(vm_);
}


int main(int argc, char *argv[]){
    try{
        parseCommandLine(argc,argv);
        if(vm_.count("help")){
            showHelp();
        }
        std::ifstream t(config);
        std::string configStr((std::istreambuf_iterator<char>(t)),
                              std::istreambuf_iterator<char>());
        BSON::Value config = BSON::Value::fromJSON(configStr);
        SusiShell shell{config["addr"],static_cast<short>((BSON::int64)config["port"]),config["key"],config["cert"],config["commands"]};
        shell.join();
    }catch(const std::exception & e){
        std::cout << e.what() << std::endl;
        showHelp();
    }
    return 0;
}
