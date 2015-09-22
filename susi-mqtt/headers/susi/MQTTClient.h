#include "leveldb/db.h"
#include "susi/SusiClient.h"
#include <mosquittopp.h>

class MQTTClient : public mosqpp::mosquittopp {

public:
	MQTTClient(std::string addr,short port, std::string key, std::string cert, std::string mqttAddr, short mqttPort);
	void subscribe(const std::string & topic);
	void forward(const std::string & topic);
	void join();

protected:
	std::shared_ptr<Susi::SusiClient> susi_;
	std::thread runloop_;
	std::vector<std::string> subscriptions_;

	void on_connect(int rc);
	void on_message(const struct mosquitto_message *message);
	void on_subscribe(int mid, int qos_count, const int *granted_qos);

};
