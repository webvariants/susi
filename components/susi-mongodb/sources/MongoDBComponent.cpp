#include "susi/MongoDBComponent.h"

Susi::MongoDBComponent::MongoDBComponent(Susi::SusiClient & susi, const BSON::Value & config) : _susi{susi}, _config{config} {

    bsoncxx::builder::stream::document document{};

    auto collection = conn["testdb"]["testcollection"];
    document << "hello" << "world";

    collection.insert_one(document.view());
    auto cursor = collection.find({});

    for (auto&& doc : cursor) {
        std::cout << bsoncxx::to_json(doc) << std::endl;
    }
}

Susi::MongoDBComponent::~MongoDBComponent(){
    join();
}

void Susi::MongoDBComponent::join(){
    _susi.join();
}
