#include "susi/SusiCluster.h"

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
        ("conf,c", po::value<std::string>(&config)->default_value("clusterconfig.json"), "address of susi instance");
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
        SusiCluster cluster{config["addr"],static_cast<short>((BSON::int64)config["port"]),config["key"],config["cert"]};
        if(config["nodes"].isArray()){
            for(std::size_t nodeId=0;nodeId < config["nodes"].size(); nodeId++){
                auto & node = config["nodes"][nodeId];
                std::cout<<"current node: "<<node.toJSON()<<std::endl;
                cluster.registerNode(node["id"],node["addr"],static_cast<short>((BSON::int64)config["port"]),node["key"],node["cert"]);
                std::cout<<"registered node"<<std::endl;
                if(node["forwardProcessors"].isArray()){
                    for(std::size_t i=0;i< node["forwardProcessors"].size();i++){
                        std::string & topic = node["forwardProcessors"][i];
                        cluster.forwardProcessorEvent(topic,node["id"]);
                    }
                }
                if(node["forwardConsumers"].isArray()){
                    for(std::size_t i=0;i< node["forwardConsumers"].size();i++){
                        std::string & topic = node["forwardConsumers"][i];
                        cluster.forwardConsumerEvent(topic,node["id"]);
                    }
                }
            }
        }
        cluster.join();
    }catch(const std::exception & e){
        std::cout << e.what() << std::endl;
        showHelp();
    }
    return 0;
}