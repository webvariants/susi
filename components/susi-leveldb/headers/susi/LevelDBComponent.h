#include "leveldb/db.h"
#include "susi/SusiClient.h"

namespace Susi {
	class LevelDBComponent {

	public:
		LevelDBComponent(Susi::SusiClient & susi, std::string dbPath);
		void join();

	protected:
		Susi::SusiClient & susi_;
		leveldb::Options dbOptions_;
		std::string dbPath_;
		std::shared_ptr<leveldb::DB> db_;

		bool validateFieldsForPut(const Susi::EventPtr & event);
		bool validateFieldsForGet(const Susi::EventPtr & event);
		bool validateFieldsForGetRange(const Susi::EventPtr & event);

		void handlePut(Susi::EventPtr event);
		void handleGet(Susi::EventPtr event);
		void handleDelete(Susi::EventPtr event);
		void handleGetPrefix(Susi::EventPtr event);
		void handleGetRange(Susi::EventPtr event);

	};
}
