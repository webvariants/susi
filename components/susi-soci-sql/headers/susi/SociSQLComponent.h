#include "susi/SusiClient.h"

// #include <bsoncxx/builder/stream/document.hpp>
// #include <bsoncxx/json.hpp>

#include "soci/soci.h"
#include "soci/sqlite3/soci-sqlite3.h"
#include <iostream>
#include <istream>
#include <ostream>
#include <exception>

namespace Susi {

    class SociSQLComponent {

    public:
        SociSQLComponent(Susi::SusiClient & susi, const BSON::Value & config);

        void create(const std::string collection, const std::string name, std::string phone);
        std::vector<BSON::Value>  find(const std::string collection, const std::string name);
        void update(const std::string collection, const std::string field, const std::string set);
        void remove(const std::string collection, const std::string field, const std::string match);

        ~SociSQLComponent();
        void join();

    protected:
        SusiClient & _susi;
        const BSON::Value & _config;
        // mongocxx::instance inst{};
        // mongocxx::client conn{mongocxx::uri{}};
        std::string username;
        std::string password;
        std::string database;
    };

}
