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
	pool.add([](){std::cout<<"1"<<std::endl;});
	pool.add([](){std::cout<<"2"<<std::endl;});
	pool.add([](){std::cout<<"3"<<std::endl;});
	pool.add([](){std::cout<<"4"<<std::endl;});
	pool.add([](){std::cout<<"5"<<std::endl;});
	pool.add([](){std::cout<<"6"<<std::endl;});
	pool.add([](){std::cout<<"7"<<std::endl;});

	return 0;
}