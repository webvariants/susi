#include "db/ManagerItem.h"

Susi::DB::ManagerItem::ManagerItem( std::string dbtype, std::string connectURI ) {
    this->connected = false;
    this->dbtype = dbtype;
    this->connectURI = connectURI;
}