#include "susi/SociSQLComponent.h"

Susi::SociSQLComponent::SociSQLComponent(Susi::SusiClient & susi, const BSON::Value & config) : _susi{susi}, _config{config} {

    try{
        username = _config["username"].getString();
        password = _config["password"].getString();
        database = _config["database"].getString();

        soci::session sql(soci::sqlite3, database);

        // std::string name = "Tim";
        // std::string phone = "4687841";
        //
        // update("phonebook", phone, name);
        // remove("phonebook", "name", name);
    }
    catch(const std::exception & e) {
        std::cout << e.what() << std::endl;
    }

    _susi.registerProcessor("soci-sql::insert", [this](Susi::EventPtr event){
        auto into = event->payload["into"].getString();
        auto data = event->payload["data"];
        insert(into, data);
        event->payload["success"] = true;
    });

    _susi.registerProcessor("soci-sql::select", [this](Susi::EventPtr event){
        auto into = event->payload["into"].getString();
        auto fields = event->payload["fields"];
        auto query = event->payload["query"];
        event->payload["data"] = select(into, fields, query);
    });

    _susi.registerProcessor("soci-sql::update", [this](Susi::EventPtr event){
        // auto into = event->payload["into"].getString();
        // auto findQuery = event->payload["findQuery"];
        // auto updateQuery = event->payload["updateQuery"];
        // update(into, findQuery, updateQuery);
        event->payload["success"] = true;
    });

    _susi.registerProcessor("soci-sql::remove", [this](Susi::EventPtr event){
        // auto into = event->payload["into"].getString();
        // auto removeQuery = event->payload["removeQuery"];
        // remove(into, removeQuery);
        event->payload["success"] = true;
    });

    _susi.registerProcessor("soci-sql::query", [this](Susi::EventPtr event){
        // auto into = event->payload["into"].getString();
        // auto removeQuery = event->payload["removeQuery"];
        // remove(into, removeQuery);
        event->payload["success"] = true;
    });
}


void Susi::SociSQLComponent::insert(const std::string & into, const BSON::Value & data) {
    sql << "insert into " << into << "(";
    BSON::Object obj = data.getObject();
    for (auto kv: obj) {
        sql << kv.first << ", ";
    }
    sql << ") values(";
    for (auto kv: obj) {
        if (kv.second.isString()) {
            sql << kv.second << ", ";
        }
        else if (kv.second.isInt64()) {
            sql << std::to_string(kv.second.getInt64()) << ", ";
        }
    }
    sql << ")";
}

BSON::Value Susi::SociSQLComponent::select(const std::string & into, const BSON::Value & fields, const BSON::Value & query) {
    soci::rowset<soci::row> data;
    soci::indicator ind;

    sql << "select ";
    BSON::Array f = fields.getArray();
    for (const auto & v: f) {
        sql << v << ", ";
    }

    sql << " from " << into << " where ";
    BSON::Object q = query.getObject();
    for (const auto & v: q) {
        sql << v.first << " = " << v.second << ", ";
    }
    sql << into(data, ind);

    if (ind == soci::i_ok) {
        BSON::Value result = BSON::Object({});
        for (const auto & row: data) {
            data.push_back(row);
        }
    }
    else {
        throw std::runtime_error{"SOCI Select: Cant select"};
    }
    return data;
}

void Susi::SociSQLComponent::update(const std::string & into, const std::string field, const std::string set) {
    sql << "update :into set phone = :set where name = :field", soci::use(field), soci::use(set), soci::use(field);
}

void Susi::SociSQLComponent::remove(const std::string & into, const std::string field, const std::string &match) {
    sql << "delete from :into where :field = ':match'", soci::use(into), soci::use(field), soci::use(match);
}

void Susi::SociSQLComponent::query(const std::string & into, const std::string field, const std::string &match) {
    // sql << "delete from :into where :field = ':match'", soci::use(into), soci::use(field), soci::use(match);
}

Susi::SociSQLComponent::~SociSQLComponent() {
    join();
}

void Susi::SociSQLComponent::join() {
    _susi.join();
}
