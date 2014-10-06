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

#ifndef __DB_MANAGER_ITEM__
#define __DB_MANAGER_ITEM__

#include <db/Database.h>

namespace Susi {
    namespace DB {
        class DBManagerItem {
        public:
            bool connected;
            std::string dbtype;
            std::string connectURI;
            Susi::Database * db;
            DBManagerItem() {};
            DBManagerItem( std::string dbtype, std::string connectURI );
        };
    }
}

#endif // __DB_MANAGER_ITEM__