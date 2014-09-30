#ifndef __DATABASE__
#define __DATABASE__

#include <soci.h>
#include <sqlite3/soci-sqlite3.h>
#include <firebird/soci-firebird.h>
#include <iostream>

#include "util/Any.h"

namespace Susi {
    namespace DB {
        class Database {
        protected:
            soci::session session;
        public:
            Database( std::string dbtype,std::string connectURI ) : session( dbtype,connectURI ) {};
            Susi::Util::Any query( std::string query );
        };
    }
}

#endif // __DATABASE__
