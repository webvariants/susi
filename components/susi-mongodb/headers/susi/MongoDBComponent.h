#include "susi/SusiClient.h"

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

#include <bcon.h>
#include <mongoc.h>

#include <vector>

namespace Susi {
    class MongoDBComponent {

	    public:
	        MongoDBComponent(Susi::SusiClient & susi, const BSON::Value & config);

	        void create(const std::string collection, const BSON::Value data);
	        std::vector<BSON::Value> find(const std::string collection, const BSON::Value query);
	        void update(const std::string collection, const BSON::Value findQuery, const BSON::Value updateQuery);
	        void remove(const std::string collection, const BSON::Value queryRemove);
			BSON::Value mapreduce(const std::string collectionName, const std::string map, const std::string reduce,
				const BSON::Value query, const BSON::Value sort, const int limit, const std::string finalize);

	        ~MongoDBComponent();
	        void join();

	    protected:
	        SusiClient & _susi;
	        const BSON::Value & _config;
	        mongocxx::instance inst{};
	        mongocxx::client conn{mongocxx::uri{}};
	        std::string host;
	        std::string port;
	        std::string username;
	        std::string password;
	        std::string database;

			mongoc_client_t* client;
			mongoc_collection_t* collection;
			bson_t* query;
			bson_t reply;
			bson_error_t error;

    };
}
