#include "susi/SusiServer.h"

Susi::SusiServer::SusiServer(short port, std::string keyFile, std::string certificateFile) :
    Susi::FramingServer<LineFramer, Susi::SSLTCPServer> {port, keyFile, certificateFile} {}

Susi::SusiServer::~SusiServer() {}

void Susi::SusiServer::onConnect(int id) {
    std::cout << "got new client " << Susi::SSLTCPServer::getPeerCertificateHash(id) << std::endl;
    Susi::FramingServer<LineFramer, Susi::SSLTCPServer>::onConnect(id);
    BSON::Value sessionNewEvent = BSON::Object{
        {"topic", "core::session::new"},
        {"payload", id}
    };
    publish(sessionNewEvent, 0);
}

void Susi::SusiServer::onClose(int id) {
    std::cout << "lost client " << Susi::SSLTCPServer::getPeerCertificateHash(id) << std::endl;
    Susi::FramingServer<LineFramer, Susi::SSLTCPServer>::onClose(id);

    //remove all publish processes associated with this client
    for (auto it = publishProcesses.cbegin(); it != publishProcesses.cend();) {
        if (it->second->publisher == id) {
            publishProcesses.erase(it++);
        } else {
            ++it;
        }
    }

    //remove all associated consumers
    for (auto & kv : consumers) {
        kv.second.erase(std::remove_if(kv.second.begin(), kv.second.end(),
        [id](int currentId) { return currentId == id; }), kv.second.end());
    }

    //remove all associated processors
    for (auto & kv : processors) {
        kv.second.erase(std::remove_if(kv.second.begin(), kv.second.end(),
        [id](int currentId) { return currentId == id; }), kv.second.end());
    }

    //cleanup publishProcesses
    std::vector<std::shared_ptr<PublishProcess>> orphanedProcesses;
    for (auto & kv : publishProcesses) {
        auto process = kv.second;
        std::cout << "process: " << process << std::endl;
        // check if there are pending processorEvents for this client
        if (process->nextProcessor - 1 < process->processors.size() && process->processors[process->nextProcessor - 1] == id) {
            orphanedProcesses.push_back(process);
        }
        //remove all id occurences from the process
        for (size_t i = process->nextProcessor; i < process->processors.size(); i++) {
            if (process->processors[i] == id) {
                process->processors.erase(process->processors.begin() + i);
                break;
            }
        }
        for (size_t i = 0; i < process->consumers.size(); i++) {
            if (process->consumers[i] == id) {
                process->consumers.erase(process->consumers.begin() + i);
                break;
            }
        }
    }
    for (auto & p : orphanedProcesses) {
        ack(p->lastState, 0);
    }
    BSON::Value sessionLostEvent = BSON::Object{
        {"topic", "core::session::lost"},
        {"payload", id}
    };
    publish(sessionLostEvent, 0);
}

void Susi::SusiServer::onFrame(int id, std::string & frame) {
    auto doc = BSON::Value::fromJSON(frame);
    if (!validateFrame(doc)) {
        BSON::Value res = BSON::Object{
            {"type", "error"},
            {
                "data", BSON::Object{
                    {"reason", "your request is invalid"}
                }
            }
        };
        send(id, res);
    } else {
        std::string & type = doc["type"];
        BSON::Value & data = doc["data"];
        if (type == "registerConsumer") {
            registerConsumer(data["topic"], id);
        }
        if (type == "registerProcessor") {
            registerProcessor(data["topic"], id);
        }
        if (type == "unregisterConsumer") {
            unregisterConsumer(data["topic"], id);
        }
        if (type == "unregisterProcessor") {
            unregisterProcessor(data["topic"], id);
        }
        if (type == "publish") {
            publish(data, id);
        }
        if (type == "ack") {
            ack(data, id);
        }
        if (type == "dismiss") {
            dismiss(data, id);
        }
    }
}

bool Susi::SusiServer::validateFrame(BSON::Value & doc) {
    if ( !doc.isObject() ||
            !doc["type"].isString() ||
            !doc["data"].isObject() ||
            !doc["data"]["topic"].isString()) {
        return false;
    }
    return true;
}

void Susi::SusiServer::send(int id, BSON::Value & doc) {
    std::string frame = doc.toJSON() + "\n";
    Susi::FramingServer<LineFramer, Susi::SSLTCPServer>::send(id, frame.c_str(), frame.size());
}

void Susi::SusiServer::registerConsumer(std::string & topic, int id) {
    if (!contains(consumers[topic], id)) {
        std::cout << "register consumer " + topic + " for " << Susi::SSLTCPServer::getPeerCertificateHash(id) << std::endl;
        consumers[topic].push_back(id);
    }
}

void Susi::SusiServer::registerProcessor(std::string & topic, int id) {
    if (!contains(processors[topic], id)) {
        std::cout << "register processor " + topic + " for " << Susi::SSLTCPServer::getPeerCertificateHash(id) << std::endl;
        processors[topic].push_back(id);
    }
}

void Susi::SusiServer::unregisterConsumer(std::string & topic, int id) {
    for (size_t i = 0; i < consumers[topic].size(); i++) {
        if (id == consumers[topic][i]) {
            std::cout << "unregister consumer " + topic + " for " << Susi::SSLTCPServer::getPeerCertificateHash(id) << std::endl;
            consumers[topic].erase(consumers[topic].begin() + i);
            break;
        }
    }
}

void Susi::SusiServer::unregisterProcessor(std::string & topic, int id) {
    for (size_t i = 0; i < processors[topic].size(); i++) {
        if (id == processors[topic][i]) {
            std::cout << "unregister processor " + topic + " for " << Susi::SSLTCPServer::getPeerCertificateHash(id) << std::endl;
            processors[topic].erase(processors[topic].begin() + i);
            break;
        }
    }
}

void Susi::SusiServer::publish(BSON::Value & event, int publisher) {
    std::cout << "publish event " + event["topic"].getString() + " for " << Susi::SSLTCPServer::getPeerCertificateHash(publisher) << std::endl;
    std::string & topic = event["topic"];
    if (!event["id"].isString()) {
        long id = std::chrono::system_clock::now().time_since_epoch().count();
        event["id"] = std::to_string(id);
    }
    if (!event["sessionid"].isString()) {
        event["sessionid"] = std::to_string(publisher);
    }

    auto peerCertHash = Susi::SSLTCPServer::getPeerCertificateHash(publisher);
    if (peerCertHash != "") {
        if (event["headers"].isArray()) {
            event["headers"].push_back(BSON::Object{{"Cert-Hash", peerCertHash}});
        } else {
            event["headers"] = BSON::Array{BSON::Object{{"Cert-Hash", peerCertHash}}};
        }
    }

    checkAndReactToSusiEvents(event);

    auto process = std::make_shared<PublishProcess>(io_service);

    int64_t timeout = checkForTimeoutHeader(event);
    if(timeout){
        process->timeout.expires_from_now(boost::posix_time::milliseconds(timeout));
        process->timeout.async_wait([this,process](const boost::system::error_code & error){
            if(!error){
                process->lastState["headers"].push_back(BSON::Object{{"Error","Timeout"}});
                dismiss(process->lastState,0);
            }
        });
    }

    process->publisher = publisher;
    for (auto & kv : processors) {
        std::regex e{kv.first};
        if (std::regex_match(topic, e)) {
            for (auto & id : kv.second) {
                if (!contains(process->processors, id)) {
                    process->processors.push_back(id);
                }
            }
        }
    }
    for (auto & kv : consumers) {
        std::regex e{kv.first};
        if (std::regex_match(topic, e)) {
            for (auto & id : kv.second) {
                if (!contains(process->consumers, id)) {
                    process->consumers.push_back(id);
                }
            }
        }
    }
    publishProcesses[event["id"].getString()] = process;
    ack(event, 0);
}

void Susi::SusiServer::ack(BSON::Value & event, int acker) {
    if (acker != 0) {
        std::cout << "got ack for event " + event["topic"].getString() + " from " << Susi::SSLTCPServer::getPeerCertificateHash(acker) << std::endl;
    }
    std::string & id = event["id"];
    if (publishProcesses.count(id) == 0) {
        return;
    }
    auto process = publishProcesses[id];
    process->lastState = event;
    if (process->nextProcessor >= process->processors.size()) {
        //processor phase finished, send to all consumers and to publisher
        if(!checkForNoConsumerHeader(event)){
            BSON::Value consumerEvent = BSON::Object{
                {"type", "consumerEvent"},
                {"data", event}
            };
            for (auto & id : process->consumers) {
                std::cout << "send consumer event " + event["topic"].getString() + " to " << Susi::SSLTCPServer::getPeerCertificateHash(id) << std::endl;
                send(id, consumerEvent);
            }
        }
        if (process->publisher != 0 && !checkForNoAckHeader(event)) {
            BSON::Value publisherAck = BSON::Object{
                {"type", "ack"},
                {"data", event}
            };
            std::cout << "send ack for event " + event["topic"].getString() + " to publisher " << Susi::SSLTCPServer::getPeerCertificateHash(process->publisher) << std::endl;
            send(process->publisher, publisherAck);
        }
        publishProcesses.erase(id);
    } else {
        if(!checkForNoProcessorHeader(event)){
            std::cout << "forward event " + event["topic"].getString() + " to " << Susi::SSLTCPServer::getPeerCertificateHash(process->processors[process->nextProcessor]) << std::endl;
            BSON::Value processorEvent = BSON::Object{
                {"type", "processorEvent"},
                {"data", event}
            };
            send(process->processors[process->nextProcessor++], processorEvent);
        }else{
            process->nextProcessor = process->processors.size();
            ack(event,acker);
        }
    }
}

void Susi::SusiServer::dismiss(BSON::Value & event, int acker) {
    if (acker != 0) {
        std::cout << "got dismiss for event " + event["topic"].getString() + " from " << Susi::SSLTCPServer::getPeerCertificateHash(acker) << std::endl;
    }
    std::string & id = event["id"];
    if (publishProcesses.count(id) == 0) {
        return;
    }
    auto process = publishProcesses[id];
    if(!checkForNoConsumerHeader(event)){
        BSON::Value consumerEvent = BSON::Object{
            {"type", "consumerEvent"},
            {"data", event}
        };
        for (auto & id : process->consumers) {
            std::cout << "send consumer event " + event["topic"].getString() + " to " << Susi::SSLTCPServer::getPeerCertificateHash(id) << std::endl;
            send(id, consumerEvent);
        }
    }
    if (process->publisher != 0 && !checkForNoAckHeader(event)) {
        BSON::Value publisherAck = BSON::Object{
            {"type", "dismiss"},
            {"data", event}
        };
        std::cout << "send dismiss for event " + event["topic"].getString() + " to publisher " << Susi::SSLTCPServer::getPeerCertificateHash(process->publisher) << std::endl;
        send(process->publisher, publisherAck);
    }
    publishProcesses.erase(id);
}


bool Susi::SusiServer::contains(std::vector<int> & vec, int elem) {
    for (auto & id : vec) {
        if (id == elem) {
            return true;
        }
    }
    return false;
}

void Susi::SusiServer::checkAndReactToSusiEvents(BSON::Value & event) {
    std::regex susiEventTopic{"core::.*"};
    if (std::regex_match(event["topic"].getString(), susiEventTopic)) {
        if (event["topic"].getString() == "core::session::getCertificate") {
            long long id = event["payload"].getInt64();
            std::string cert = Susi::SSLTCPServer::getPeerCertificate(id);
            event["payload"] = cert;
        }
    }
}

bool Susi::SusiServer::checkForHeader(BSON::Value & event, const std::string & key, const std::string & value){
    if(event["headers"].isArray()){
        for(auto & header : event["headers"].getArray()){
            if(header[key].isString() && header[key].getString() == value ){
                return true;
            }
        }
    }
    return false;
}
int64_t Susi::SusiServer::checkForTimeoutHeader(BSON::Value & event){
    if(event["headers"].isArray()){
        for(auto & header : event["headers"].getArray()){
            if(header["Event-Timeout"].isString() && header["Event-Timeout"].isString() ){
                return std::atoi(header["Event-Timeout"].getString().c_str());
            }
        }
    }
    return 0;
}
bool Susi::SusiServer::checkForNoAckHeader(BSON::Value & event){
    return checkForHeader(event,"Event-Control","No-Ack");
}
bool Susi::SusiServer::checkForNoConsumerHeader(BSON::Value & event){
    return checkForHeader(event,"Event-Control","No-Consumer");
}
bool Susi::SusiServer::checkForNoProcessorHeader(BSON::Value & event){
    return checkForHeader(event,"Event-Control","No-Processor");
}
