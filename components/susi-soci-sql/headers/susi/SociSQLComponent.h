#include "susi/SusiClient.h"

#include "soci/soci.h"
#include "soci/sqlite3/soci-sqlite3.h"
#include <iostream>
#include <exception>

namespace Susi {

    class SociSQLComponent {

    public:
        SociSQLComponent(Susi::SusiClient & susi, const BSON::Value & config);

        void insert(const std::string & into, const BSON::Value & data);
        BSON::Value select(const std::string & into, const BSON::Value & fields, const BSON::Value & query);
        void update(const std::string & into, const std::string field, const std::string set);
        void remove(const std::string & into, const std::string field, const std::string &match);
        void query(const std::string & into, const std::string field, const std::string &match);

        ~SociSQLComponent();
        void join();

    protected:
        SusiClient & _susi;
        const BSON::Value & _config;
        soci::session sql;

        std::string username;
        std::string password;
        std::string database;
    };

}
