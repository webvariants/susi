#include <iostream>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include "JSEngine.h"

int main(int argc, char** argv){
	if(argc < 2){
		std::cout<<"usage : "<<argv[0]<<" <sourcefile>"<<std::endl;
		std::cout<<"sample: "<<argv[0]<<" sourcefile "<<std::endl;
		return 1;
	}

	std::shared_ptr<Susi::Util::Channel<Susi::JS::V8::EngineToBridgeMessage>> bridge_in{new Susi::Util::Channel<Susi::JS::V8::EngineToBridgeMessage>()};
	std::shared_ptr<Susi::Util::Channel<Susi::JS::V8::BridgeToEngineMessage>> engine_in{new Susi::Util::Channel<Susi::JS::V8::BridgeToEngineMessage>()};

	Susi::JS::V8::Engine engine{engine_in, bridge_in};
	Susi::JS::V8::EngineBridge enginebridge{bridge_in, engine_in};

	enginebridge.start();
	engine.runFile(std::string{argv[1]});

	bool stop = false;
	std::mutex mutex;
	std::condition_variable stop_cond;
	std::unique_lock<std::mutex> lock{mutex};
	stop_cond.wait_for(lock, std::chrono::seconds{5},[&stop](){return false;});

}

