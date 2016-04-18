#include "susi/MongoDBComponent.h"

Susi::MongoDBComponent::MongoDBComponent(Susi::SusiClient & susi, const BSON::Value & config) : _susi{susi}, _config{config} {

    try{
        host     = _config["host"].getString();
        port     = _config["port"].getString();
        username = _config["username"].getString();
        password = _config["password"].getString();
        database = _config["database"].getString();

		mongoc_init();
		client = mongoc_client_new("mongodb://127.0.0.1:27017/");
		collection = mongoc_client_get_collection(client, "test", "test");
		query = BCON_NEW("cmpxchg", BCON_INT32(1));

		// if (mongoc_collection_find_and_modify(collection, query, NULL, NULL, NULL, false, false, true, &reply, &error)) {
		// 	std::cout << &reply << std::endl;
		// } else {
		// 	std::cout << &error << std::endl;
		// }
    }
    catch(const std::exception & e) {
        std::cout << e.what() << std::endl;
    }

    _susi.registerProcessor("mongodb::create", [this](Susi::EventPtr event){
        auto collection = event->payload["collection"].getString();
        auto data       = event->payload["data"];

        create(collection, data);
        event->payload["success"] = true;
    });

    _susi.registerProcessor("mongodb::find", [this](Susi::EventPtr event){
        auto collection = event->payload["collection"].getString();
        auto query      = event->payload["query"];

        event->payload["data"] = find(collection, query);
    });

    _susi.registerProcessor("mongodb::update", [this](Susi::EventPtr event){
        auto collection  = event->payload["collection"].getString();
        auto findQuery   = event->payload["findQuery"];
        auto updateQuery = event->payload["updateQuery"];

        update(collection, findQuery, updateQuery);
        event->payload["success"] = true;
    });

    _susi.registerProcessor("mongodb::remove", [this](Susi::EventPtr event){
        auto collection  = event->payload["collection"].getString();
        auto removeQuery = event->payload["removeQuery"];

        remove(collection, removeQuery);
        event->payload["success"] = true;
    });

	_susi.registerProcessor("mongodb::mapreduce", [this](Susi::EventPtr event){
        auto collection = event->payload["collection"].getString();
		auto map        = event->payload["map"].getString();
		auto reduce     = event->payload["reduce"].getString();
        auto query      = event->payload["query"].getObject();
		auto sort       = event->payload["sort"].getObject();
		auto limit      = event->payload["limit"].getInt64();
		auto finalize   = event->payload["finalize"].getString();

		event->payload["data"] = mapreduce(collection, map, reduce, query, sort, limit, finalize);
    });
}


void Susi::MongoDBComponent::create(const std::string collectionName, const BSON::Value data) {
    bsoncxx::document::value document = bsoncxx::from_json(data.toJSON());
    auto collection = conn[database][collectionName];
    collection.insert_one(document.view());
}

std::vector<BSON::Value> Susi::MongoDBComponent::find(const std::string collectionName, const BSON::Value query) {
    bsoncxx::document::value document = bsoncxx::from_json(query.toJSON());
    auto collection = conn[database][collectionName];
    auto cursor = collection.find(document.view());

    std::vector<BSON::Value> data;
    for (auto&& doc : cursor) {
        data.push_back(BSON::Value::fromJSON(bsoncxx::to_json(doc)));
    }
    return data;
}

void Susi::MongoDBComponent::update(const std::string collectionName, const BSON::Value findQuery, const BSON::Value updateQuery) {
    bsoncxx::document::value update = bsoncxx::from_json(updateQuery.toJSON());
    bsoncxx::document::value find   = bsoncxx::from_json(findQuery.toJSON());

    auto collection = conn[database][collectionName];
    collection.update_many(find.view(), update.view());
}

void Susi::MongoDBComponent::remove(const std::string collectionName, const BSON::Value removeQuery) {
    bsoncxx::document::value removeMany = bsoncxx::from_json(removeQuery.toJSON());

    auto collection = conn[database][collectionName];
    collection.delete_many(removeMany.view());
}

BSON::Value Susi::MongoDBComponent::mapreduce(
	const std::string collectionName, const std::string map, const std::string reduce,
	const BSON::Value query, const BSON::Value sort, const int limit, const std::string finalize) {

    // auto collection = conn[database];

	std::string doc = "{";
	doc += "\"mapReduce\": \"";
	doc += collectionName;
	doc += "\",";

	doc += "\"map\": ";
	doc += map;
	doc += ",";

	doc += "\"reduce\": ";
	doc += reduce;
	doc += ",";

	doc += query.toJSON();
	doc += ",";

	doc += "\"out\": {\"replace\": \"susiOut\" }";
	doc += "}";

	// auto result = collection.run_command(bsoncxx::from_json(doc));
	auto result = bsoncxx::from_json("{}");

	bson_t cmd;
	bson_init(&cmd);
	bson_append_utf8(&cmd, "mapReduce", collectionName);
	// cmd = BCON_NEW("mapReduce", collectionName);
	// cmd = BCON_NEW("map", BCON_UTF8(map));
	// cmd = BCON_NEW("reduce", BCON_UTF8(reduce));

	if (mongoc_collection_command_simple(collection, cmd, NULL, &reply, &error)) {

	} else {

	}


	// bson cmd;
	// bson_init(&cmd);
	// bson_append_string(&cmd, "map", map);
	// bson_append_string(&cmd, "reduce", reduce);
	// bson_append_string(&cmd, "query", query);
	// bson_append_string(&cmd, "sort", sort);
	// bson_append_string(&cmd, "limit", limit);
	// bson_append_string(&cmd, "finalize", finalize);
	// bson_finish(&cmd);
	//
	// if (mongoc_collection_command_simple(collection, &cmd, NULL, &reply, &error)) {
	//
	// } else {
	//
	// }
	// bson_destroy(&cmd);

    return bsoncxx::to_json(result);
}

Susi::MongoDBComponent::~MongoDBComponent() {
    join();
}

void Susi::MongoDBComponent::join() {
    _susi.join();
}
