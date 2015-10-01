#include "susi/HeartBeat.h"

HeartBeat::HeartBeat(std::string addr,short port, std::string key, std::string cert) :
  susi_{new Susi::SusiClient{addr,port,key,cert}} {
      runloop_ = std::move(std::thread{[this](){
          int count = 0;
          std::chrono::milliseconds interval{100};
          while( !this->stopVar_.load() ) {
              ++count %= 3000;
              if( count % 10 == 0) {
                  susi_->publish( susi_->createEvent( "heartbeat::one" ) );
              }
              if( count % 50 == 0 ) {
                  susi_->publish( susi_->createEvent( "heartbeat::five" ) );
              }
              if( count % 100 == 0 ) {
                  susi_->publish( susi_->createEvent( "heartbeat::ten" ) );
              }
              if( count % 600 == 0 ) {
                  susi_->publish( susi_->createEvent( "heartbeat::minute" ) );
              }
              if( count % 3000 == 0 ) {
                  susi_->publish( susi_->createEvent( "heartbeat::fiveMinute" ) );
              }
              std::this_thread::sleep_for(interval);
          }
      }});
}

void HeartBeat::join(){
	susi_->join();
}
