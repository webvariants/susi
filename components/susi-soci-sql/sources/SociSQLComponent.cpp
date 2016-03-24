#include "susi/SociSQLComponent.h"

Susi::SociSQLComponent::SociSQLComponent(Susi::SusiClient & susi, const BSON::Value & config) : _susi{susi}, _config{config} {

    try{
        username = _config["username"].getString();
        password = _config["password"].getString();
        database = _config["database"].getString();

        soci::session sql(soci::sqlite3, database);

        std::string name = "Tim";
        std::string phone = "4687841";

        create("phonebook", name, phone);
        find("phonebook", name);
        update("phonebook", phone, name);
        delete("phonebook", "name", name);
    }
    catch(const std::exception & e) {
        std::cout << e.what() << std::endl;
    }

    // _susi.registerProcessor("mongodb::create", [this](Susi::EventPtr event){
    //   auto collection = event->payload["collection"].getString();
    //   auto data = event->payload["data"];
    //   create(collection, data);
    //   event->payload["success"] = true;
    // });
    //
    // _susi.registerProcessor("mongodb::find", [this](Susi::EventPtr event){
    //   auto collection = event->payload["collection"].getString();
    //   auto query = event->payload["query"];
    //   event->payload["data"] = find(collection, query);
    // });
    //
    // _susi.registerProcessor("mongodb::update", [this](Susi::EventPtr event){
    //   auto collection = event->payload["collection"].getString();
    //   auto findQuery = event->payload["findQuery"];
    //   auto updateQuery = event->payload["updateQuery"];
    //   update(collection, findQuery, updateQuery);
    //   event->payload["success"] = true;
    // });
    //
    // _susi.registerProcessor("mongodb::remove", [this](Susi::EventPtr event){
    //   auto collection = event->payload["collection"].getString();
    //   auto removeQuery = event->payload["removeQuery"];
    //   remove(collection, removeQuery);
    //   event->payload["success"] = true;
    // });
}


void Susi::SociSQLComponent::create(const std::string collectionName, const std::string name, const std::string phone) {
    sql << "insert into :collectionName(name, phone) values(?, ?)", soci::use(collectionName), soci::use(name), soci::use(phone);
}

std::vector<BSON::Value> Susi::SociSQLComponent::find(const std::string collectionName, const std::string name) {
    std::string phone;
    std::vector<std::string> data;
    indicator ind;

    sql << "select phone from :collectionName where name = :name", into(phone, ind), use(collectionName) use(name);

    if (ind == i_ok) {
        data.push_back(phone);
    }
    else {
        data.push_back("404");
    }
    return data;
}

void Susi::SociSQLComponent::update(const std::string collectionName, const std::string field, const std::string set) {
    sql << "update :collectionName set phone = :set where name = :field", use(phonebook), use(set), use(field);
}

void Susi::SociSQLComponent::remove(const std::string collectionName, const std::string field, const std::string &match) {
    sql << "delete from :collectionName where :field = ':match'", use(collectionName), use(field), use(match);
}

Susi::SociSQLComponent::~SociSQLComponent() {
    join();
}

void Susi::SociSQLComponent::join() {
    _susi.join();
}
