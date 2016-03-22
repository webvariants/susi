#include "susi/SusiClient.h"

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

namespace Susi {

    class MongoDBComponent {

    public:
        MongoDBComponent(Susi::SusiClient & susi, const BSON::Value & config);
        ~MongoDBComponent();
        void join();

    protected:
        SusiClient & _susi;
        const BSON::Value & _config;
        mongocxx::instance inst{};
        mongocxx::client conn{mongocxx::uri{}};
    };

}
