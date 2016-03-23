#include "susi/SusiClient.h"

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

#include <vector>

namespace Susi {

    class MongoDBComponent {

    public:
        MongoDBComponent(Susi::SusiClient & susi, const BSON::Value & config);

        void create(const std::string collection, const BSON::Value data);
        std::vector<BSON::Value>  find(const std::string collection, const BSON::Value query);
        void update(const std::string collection, const BSON::Value findQuery, const BSON::Value updateQuery);
        void remove(const std::string collection, const BSON::Value queryRemove);

        ~MongoDBComponent();
        void join();

    protected:
        SusiClient & _susi;
        const BSON::Value & _config;
        mongocxx::instance inst{};
        mongocxx::client conn{mongocxx::uri{}};
        std::string host     ;
        std::string port     ;
        std::string username ;
        std::string password ;
        std::string database ;
    };

}
