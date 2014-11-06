/*
 * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 *
 * @author: Thomas Krause (thomas.krause@webvariants.de)
 */

#ifndef __DB_MANAGER__
#define __DB_MANAGER__

#include <soci.h>
#include <sqlite3/soci-sqlite3.h>
//#include <iostream>
#include <map>
#include <tuple>
#include <memory>

#include <db/Database.h>
#include <db/ManagerItem.h>
#include "logger/easylogging++.h"

namespace Susi {
    namespace DB {
        class Manager {
        protected:
            std::map <std::string, Susi::DB::ManagerItem> dbMap;
        public:
            Manager();
            Manager( Susi::Util::Any config );
            Manager( std::vector<std::tuple<std::string,std::string,std::string>> dbs );

            void init( Susi::Util::Any config );
            void addDatabase( std::string identifier,std::string dbtype,std::string connectURI );
            std::shared_ptr<Database> getDatabase( std::string identifier );
        };

    }
}

#endif // __DB_MANAGER__