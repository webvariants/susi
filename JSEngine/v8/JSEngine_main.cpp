#include <iostream>
#include <mutex>
#include <condition_variable>
#include "JSEngine.h"

int main(int argc, char** argv){
	if(argc < 3){
		std::cout<<"usage : "<<argv[0]<<" <susi address> <source file>"<<std::endl;
		std::cout<<"sample: "<<argv[0]<<" [::1]:4000 sourcefile.js"<<std::endl;
		return 1;			
	}

	Susi::JS::Engine engine{argv[1],argv[2]};

	bool stop = false;
	std::mutex mutex;
	std::condition_variable stop_cond;
	std::unique_lock<std::mutex> lock{mutex};
	stop_cond.wait(lock,[&stop](){return stop;});

}
