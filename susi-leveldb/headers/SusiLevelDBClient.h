#include "leveldb/db.h"
#include "susi/SusiClient.h"

class SusiLevelDBClient {

public:
	SusiLevelDBClient(std::string addr,short port, std::string key, std::string cert, std::string dbPath);
	void join();

protected:
	std::shared_ptr<Susi::SusiClient> susi_;
	leveldb::Options dbOptions_;
	std::string dbPath_;
	std::shared_ptr<leveldb::DB> db_;

	bool validateFieldsForPut(const BSON::Value & event);
	bool validateFieldsForGet(const BSON::Value & event);

	void handlePut(BSON::Value & event);
	void handleGet(BSON::Value & event);
	void handleDelete(BSON::Value & event);

};

