#include "susi/SusiShell.h"
#include "susi/process/process.hpp"

SusiShell::SusiShell(std::string addr,short port, std::string key, std::string cert, BSON::Object commands) :
  _susi{new Susi::SusiClient{addr,port,key,cert}}, _commands{commands} {
    _susi->registerProcessor("shell::exec",[this](Susi::EventPtr event){
        handleExecEvent(std::move(event));
    });
}

void SusiShell::join(){
    _susi->join();
}

void SusiShell::handleExecEvent(Susi::EventPtr event){
    if(!event->payload.isObject() ||
       !event->payload["command"].isString()){
           throw std::runtime_error{"SusiShell: need 'command' in payload"};
    }
    std::string command = event->payload["command"].getString();
    if(_commands.count(command) == 0){
        throw std::runtime_error{"SusiShell: no valid command, check your config file"};
    }
    command = _commands[command].getString();
    if(event->payload["args"].isObject()){
        BSON::Object args = event->payload["args"].getObject();
        for(auto & kv : args){
            size_t pos = std::string::npos;
            if( (pos = command.find("$"+kv.first)) != std::string::npos){
                if(!kv.second.isString()){
                    throw std::runtime_error{"SusiShell: all arguments need to be strings"};
                }
                std::string replacement = kv.second.getString();
                command.replace(pos,kv.first.size()+1,replacement);
            }
        }
    }
    std::string stdin{""}, stdout{""}, stderr{""};
    if(event->payload["stdin"].isString()){
        stdin = event->payload["stdin"].getString();
    }
    handleExec(command,stdin,stdout,stderr);
    event->payload["stdout"] = stdout;
    event->payload["stderr"] = stderr;
}

void SusiShell::handleExec(std::string command,
                std::string & stdin,
                std::string & stdout,
                std::string & stderr){
    fr::process proc{command};
    proc.start();
    proc.in->stream_out << stdin;
    proc.join();
    std::string line;
    while(std::getline(proc.out->stream_in, line)){
        stdout += line + '\n';
    }
    while(std::getline(proc.err->stream_in, line)){
        stderr += line + '\n';
    }
}
