#include "susi/SusiClient.h"

class SusiStateFileClient {

public:
	SusiStateFileClient(std::string addr,short port, std::string key, std::string cert, std::string stateFile);
	void join();

protected:
	std::shared_ptr<Susi::SusiClient> susi_;
	std::string stateFile_;
	BSON::Value state_;

	bool validateFieldsForPut(const Susi::EventPtr & event);
	bool validateFieldsForGet(const Susi::EventPtr & event);

	void handlePut(Susi::EventPtr event);
	void handleGet(Susi::EventPtr event);
	void handleDelete(Susi::EventPtr event);

	bool readStateFile();
	bool writeStateFile();

};
