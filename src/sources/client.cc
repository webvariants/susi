#include <iostream>
#include <mutex>
#include <condition_variable>
#include "apiserver/ApiClient.h"
int main(){
	using namespace Susi::Api;
	using namespace Susi::Events;
	
	ApiClient client{"[::1]:4000"};
	
	Processor alarmsHandler{[](EventPtr event){
		std::cout<<"got event!"<<std::endl;
		event->headers.push_back(std::make_pair("foo","bar"));
	}};
	
	client.subscribe("test",alarmsHandler);
	
	std::mutex mutex;
	std::condition_variable cond;
	std::unique_lock<std::mutex> lock{mutex};
	cond.wait(lock);
}
