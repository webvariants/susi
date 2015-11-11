#include "susi/HeartBeat.h"

HeartBeat::HeartBeat(std::string addr,short port, std::string key, std::string cert) :
  susi_{new Susi::SusiClient{addr,port,key,cert}} {
      runloop_ = std::move(std::thread{[this](){
          int count = 0;
          std::chrono::milliseconds interval{100};
          while( !this->stopVar_.load() ) {
              ++count %= 3000;
              if( count % 10 == 0) {
                  auto event = susi_->createEvent("heartbeat::one");
                  event->headers.push_back({"Event-Control","No-Ack"});
                  susi_->publish(std::move(event));
              }
              if( count % 50 == 0 ) {
                  auto event = susi_->createEvent("heartbeat::five");
                  event->headers.push_back({"Event-Control","No-Ack"});
                  susi_->publish(std::move(event));
              }
              if( count % 100 == 0 ) {
                  auto event = susi_->createEvent("heartbeat::ten");
                  event->headers.push_back({"Event-Control","No-Ack"});
                  susi_->publish(std::move(event));
              }
              if( count % 600 == 0 ) {
                  auto event = susi_->createEvent("heartbeat::minute");
                  event->headers.push_back({"Event-Control","No-Ack"});
                  susi_->publish(std::move(event));
              }
              if( count % 3000 == 0 ) {
                  auto event = susi_->createEvent("heartbeat::fiveMinute");
                  event->headers.push_back({"Event-Control","No-Ack"});
                  susi_->publish(std::move(event));
              }
              std::this_thread::sleep_for(interval);
          }
      }});
}

void HeartBeat::join(){
	susi_->join();
}
