#include <iostream>
#include <mutex>
#include <chrono>
#include <condition_variable>

#include "engine/Engine.h"

int main(int argc, char** argv){

	std::shared_ptr<Susi::Cpp::Engine> e{new Susi::Cpp::Engine{}};
	e->populateController();
	e->start();
	e->getApi()->publish(std::move(e->getApi()->createEvent("samplecontroller::test")));

	std::condition_variable cond;
	std::mutex mutex;
	std::unique_lock<std::mutex> lock{mutex};
	cond.wait(lock, [](){return false;});
}
