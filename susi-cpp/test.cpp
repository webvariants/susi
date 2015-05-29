#include "headers/SSLClient.h"

class Test : public Susi::SSLClient {
public:
	Test(std::string host, short port,std::string key,std::string cert) : SSLClient{host,port,key,cert} {}

	virtual void onConnect() override {
		std::cout<<"connected"<<std::endl;
		std::string msg = "{\"type\":\"publish\",\"data\":{\"topic\":\"foobar\"}}";
		send(msg);
	}

	virtual void onData(char * data, size_t len) override {
		std::cout<<std::string{data,len};
	}

	virtual void onClose() override {
		std::cout<<"closed"<<std::endl;
	}
};

int main(){
	Test client{"localhost",4000,"server.key","server.cert"};
	client.join();
	return 0;
}