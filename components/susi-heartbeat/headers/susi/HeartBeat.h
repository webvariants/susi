#include "susi/SusiClient.h"

class HeartBeat {

public:
	HeartBeat(std::string addr,short port, std::string key, std::string cert);
	void join();

protected:
	std::shared_ptr<Susi::SusiClient> susi_;
	std::thread runloop_;
	std::atomic<bool> stopVar_{false};
};
