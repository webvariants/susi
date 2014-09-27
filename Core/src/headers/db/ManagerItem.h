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

#ifndef __MANAGER_ITEM__
#define __MANAGER_ITEM__

#include <map>
#include <db/Database.h>

namespace Susi {
    namespace DB {
        class ManagerItem {
        public:
            bool connected;
            std::string dbtype;
            std::string connectURI;
            std::shared_ptr<Susi::DB::Database> db;
            ManagerItem() {};
            ManagerItem( std::string dbtype, std::string connectURI );
        };
    }
}

#endif // __MANAGER_ITEM__