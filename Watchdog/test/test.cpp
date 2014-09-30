#include <iostream>
#include <csignal>
#include <mutex>
#include <condition_variable>
#include <thread>

void signalHandler (int signum) {
	std::cout << "Test: interrupt signal (" << signum << ") received."<<std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds{1000});
}

int main(){
	for(int i=1;i<32;i++){
		signal(i, signalHandler);
	}
	std::mutex mutex;
	std::condition_variable cond;
	{
		std::unique_lock<std::mutex> lock{mutex};
		cond.wait(lock,[](){return false;});
	}
}