#include <iostream>
#include <mutex>
#include <chrono>
#include <condition_variable>

#include "engine/Engine.h"

int main(int argc, char** argv){
	if(argc < 2){
		std::cout<<"usage : "<<argv[0]<<" <sourcefile>"<<std::endl;
		std::cout<<"sample: "<<argv[0]<<" sourcefile "<<std::endl;
		return 1;
	}


	std::shared_ptr<Susi::Cpp::Engine> e{Susi::Cpp::Engine::getInstance()};
	e->populateController();
	e->getApi()->publish(std::move(e->getApi()->createEvent("samplecontroller::test")));

}
