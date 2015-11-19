#include "susi/SusiClient.h"
#include "server_wss.hpp"

typedef SimpleWeb::SocketServer<SimpleWeb::WSS> WssServer;

class SusiHTTP {

public:
	SusiHTTP(std::string addr, unsigned short port, std::string key, std::string cert, unsigned short httpPort);
	void join();

protected:
	std::shared_ptr<Susi::SusiClient> susi_;
	std::thread runloop_;
	WssServer server_;
	std::atomic<bool> stopVar_{false};

	void onMessage(std::shared_ptr<WssServer::Connection> connection, std::shared_ptr<WssServer::Message> message);
	void onOpen(std::shared_ptr<WssServer::Connection> connection);
	void onClose(std::shared_ptr<WssServer::Connection> connection, int status, const std::string& reason);
	void onError(std::shared_ptr<WssServer::Connection> connection, const boost::system::error_code& ec);
};
