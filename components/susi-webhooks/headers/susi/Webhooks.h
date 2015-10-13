#include "leveldb/db.h"
#include "susi/SusiClient.h"

class Webhooks {

public:
	Webhooks(std::string addr,short port, std::string key, std::string cert);
	void join();

protected:
	std::shared_ptr<Susi::SusiClient> susi_;
	boost::asio::io_service io_service_;
};
