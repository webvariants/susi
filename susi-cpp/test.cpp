#include "susi/SusiClient.h"

#include "susi/ThreadPool.h"

class Test : public Susi::SusiClient {
public:
	Test(std::string host, short port,std::string key,std::string cert) : SusiClient{host,port,key,cert} {}

	virtual void onConnect() override {
		std::cout<<"connected"<<std::endl;
		std::string msg = "{\"type\":\"publish\",\"data\":{\"topic\":\"foobar\"}}";
		send(msg);
	}

	virtual void onFrame(std::string & frame) override {
		std::cout<<"special onFrame: "<<frame<<std::endl;
	}

	virtual void onClose() override {
		std::cout<<"closed"<<std::endl;
	}
};

int main(){
	Susi::ThreadPool pool{4};
	pool.add([](){std::cout<<"task 1"<<std::endl;});
	pool.add([](){std::cout<<"task 2"<<std::endl;});
	pool.add([](){std::cout<<"task 3"<<std::endl;});
	pool.add([](){std::cout<<"task 4"<<std::endl;});
	pool.add([](){std::cout<<"task 5"<<std::endl;});
	pool.add([](){std::cout<<"task 6"<<std::endl;});
	pool.add([](){std::cout<<"task 7"<<std::endl;});

	std::this_thread::sleep_for(std::chrono::seconds{1});

	return 0;
}