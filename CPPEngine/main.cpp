#include <iostream>
#include <mutex>
#include <chrono>
#include <condition_variable>

int main(int argc, char** argv){
	if(argc < 2){
		std::cout<<"usage : "<<argv[0]<<" <sourcefile>"<<std::endl;
		std::cout<<"sample: "<<argv[0]<<" sourcefile "<<std::endl;
		return 1;
	}

}
