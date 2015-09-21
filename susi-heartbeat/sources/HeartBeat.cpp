#include "susi/HeartBeat.h"

HeartBeat::HeartBeat(std::string addr,short port, std::string key, std::string cert) :
  susi_{new Susi::SusiClient{addr,port,key,cert}} {
      runloop_ = std::move(std::thread{[this](){
        while(true){
            std::this_thread::sleep_for(std::chrono::seconds{1});
            auto event = susi_->createEvent("core::heartbeat");
            susi_->publish(std::move(event));
            std::cout<<"published heartbeat"<<std::endl;
        }
      }});
}

void HeartBeat::join(){
	susi_->join();
}
