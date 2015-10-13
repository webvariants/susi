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

	bool validateFieldsForPut(const Susi::EventPtr & event);
	bool validateFieldsForGet(const Susi::EventPtr & event);

	void handlePut(Susi::EventPtr event);
	void handleGet(Susi::EventPtr event);
	void handleDelete(Susi::EventPtr event);

};

