#include <iostream>
#include "susi/SusiServer.h"

std::string progName = "";

short port = 4000;
std::string keyFile = "server.key";
std::string certFile = "server.cert";

void showHelp(){
    std::cout<<"usage: "<<progName<<std::endl;
    std::cout<<"\t"<<"--port <port number>"<<std::endl;
    std::cout<<"\t"<<"--cert <certificate file>"<<std::endl;
    std::cout<<"\t"<<"--key <key file>"<<std::endl;
    exit(0);
}

void parseCommandLine(int argc, char **argv){
    progName = argv[0];
    for(int i=1;i<argc;i++){
        if(std::strcmp(argv[i],"--port")==0){
            if(i+1<argc){
                port = std::atoi(argv[i+1]);
                i++;
            }else{
                showHelp();
            }
        }else if(std::strcmp(argv[i],"--key")==0){
            if(i+1<argc){
                keyFile = argv[i+1];
                i++;
            }else{
                showHelp();
            }
        }else if(std::strcmp(argv[i],"--cert")==0){
            if(i+1<argc){
                certFile = argv[i+1];
                i++;
            }else{
                showHelp();
            }
        }else{
            showHelp();
        }
    }
}

int main(int argc, char **argv){
    parseCommandLine(argc,argv);
    try{
        std::cout<<"starting susi server on port "<<port<<std::endl;
        std::cout<<"using ssl certificate "<<certFile<<std::endl;
        std::cout<<"using ssl key "<<keyFile<<std::endl;
        Susi::SecureSusiServer server{port,keyFile,certFile};
        server.join();
    }catch(const std::exception & e){
        std::cerr<<e.what()<<std::endl;
        showHelp();
    }
}
