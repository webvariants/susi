#include "susi/HeartBeat.h"

Susi::HeartBeat::HeartBeat(Susi::SusiClient & susi) :
  susi_{susi} {
      runloop_ = std::move(std::thread{[this](){
          int count = 0;
          std::chrono::milliseconds interval{100};
          while( !this->stopVar_.load() ) {
              ++count %= 3000;
              if( count % 10 == 0) {
                  auto event = susi_.createEvent("heartbeat::one");
                  event->headers.push_back({"Event-Control","No-Ack"});
                  susi_.publish(std::move(event));
              }
              if( count % 50 == 0 ) {
                  auto event = susi_.createEvent("heartbeat::five");
                  event->headers.push_back({"Event-Control","No-Ack"});
                  susi_.publish(std::move(event));
              }
              if( count % 100 == 0 ) {
                  auto event = susi_.createEvent("heartbeat::ten");
                  event->headers.push_back({"Event-Control","No-Ack"});
                  susi_.publish(std::move(event));
              }
              if( count % 600 == 0 ) {
                  auto event = susi_.createEvent("heartbeat::minute");
                  event->headers.push_back({"Event-Control","No-Ack"});
                  susi_.publish(std::move(event));
              }
              if( count % 3000 == 0 ) {
                  auto event = susi_.createEvent("heartbeat::fiveMinute");
                  event->headers.push_back({"Event-Control","No-Ack"});
                  susi_.publish(std::move(event));
              }
              std::this_thread::sleep_for(interval);
          }
      }});
}

void Susi::HeartBeat::join(){
	susi_.join();
}
