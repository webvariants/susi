#include "susi/Authenticator.h"

Authenticator::Authenticator(std::string addr,short port, std::string key, std::string cert) :
  susi_{new Susi::SusiClient{addr,port,key,cert}} {
      load();
}

void Authenticator::addUser(std::shared_ptr<User> user){
    usersByName[user->name] = user;
}

void Authenticator::addPermission(Permission permission){
    permissionsByTopic[permission.pattern.topic] = permission;
}

void Authenticator::load(){
    auto userRequestEvent = susi_->createEvent("state::get");
    userRequestEvent->payload["key"] = "authenticator::users";
    susi_->publish(std::move(userRequestEvent),[this](Susi::SharedEventPtr event){
        auto & users = event->payload["value"];
        for(size_t i=0;i<users.size();i++){
            auto & user = users[i];
            auto u = std::shared_ptr<User>{ new User{
                .name = user["name"].getString(),
                .pwHash = user["pwHash"].getString(),
                .token = ""
            }};
            for(size_t j=0;j<user["roles"].size();j++){
                u->roles.push_back(user["roles"][j].getString());
            }
            addUser(u);
        }
    });
    auto permissionRequestEvent = susi_->createEvent("state::get");
    permissionRequestEvent->payload["key"] = "authenticator::permissions";
    susi_->publish(std::move(permissionRequestEvent),[this](Susi::SharedEventPtr event){
        auto & permissions = event->payload["value"];
        for(size_t i=0;i<permissions.size();i++){
            auto & permission = permissions[i];
            Permission p = {
                .pattern = Susi::Event{permission["pattern"]}
            };
            for(size_t j=0;j<permission["roles"].size();j++){
                p.roles.push_back(permission["roles"][j].getString());
            }
            addPermission(p);
        }
    });
}

void Authenticator::save(){
    BSON::Array userArray;
    for(auto & kv : usersByName){
        auto & user = kv.second;
        userArray.push_back(BSON::Object{
            {"name",user->name},
            {"pwHash",user->pwHash},
            {"roles",BSON::Array{
                user->roles.begin(),
                user->roles.end()
            }}
        });
    }
    auto userSaveEvent = susi_->createEvent("state::put");
    userSaveEvent->payload["key"] = "authenticator::users";
    userSaveEvent->payload["value"] = userArray;
    susi_->publish(std::move(userSaveEvent));

    BSON::Array permissionArray;
    for(auto & kv : permissionsByTopic){
        auto & permission = kv.second;
        permissionArray.push_back(BSON::Object{
            {"pattern",permission.pattern.toAny()},
            {"roles",BSON::Array{
                permission.roles.begin(),
                permission.roles.end()
            }}
        });
    }
    auto permissionSaveEvent = susi_->createEvent("state::put");
    permissionSaveEvent->payload["key"] = "authenticator::permissions";
    permissionSaveEvent->payload["value"] = userArray;
    susi_->publish(std::move(permissionSaveEvent));
}

void Authenticator::join(){
	susi_->join();
}

std::string Authenticator::generateToken(){
    std::string token;
    size_t len = 64;
    token.reserve(len);

    static const std::string alphanums =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    static std::mt19937 rg(std::chrono::system_clock::now().time_since_epoch().count());
    static std::uniform_int_distribution<> pick(0, alphanums.size() - 1);

    for (int i = 0; i < len; ++i) {
        token += alphanums[pick(rg)];
    }
    return token;
}

std::string Authenticator::getTokenFromEvent(const Susi::EventPtr & event){
    for(const auto & header : event->headers){
        if(header.first == "authenticator::token"){
            return header.second;
        }
    }
    return "";
}

void Authenticator::login(Susi::EventPtr event){
    SHA3 sha3;
    auto name = event->payload["username"].getString();
    auto pwHash = sha3(event->payload["password"].getString());

    // check username and password
    if(usersByName.count(name) == 0){
        throw std::runtime_error{"no such user"};
    }
    auto & user = usersByName[name];
    if(pwHash != user->pwHash){
        std::this_thread::sleep_for(std::chrono::milliseconds{100}); //this is agains brute forcers
        throw std::runtime_error{"wrong password"};
    }
    user->token = generateToken();
    event->payload["token"] = user->token;
    usersByToken[user->token] = user;

    // prevent arbitary consumers from reading sensitive data
    event->headers.push_back({"Event-Control","No-Consumer"});
    susi_->ack(std::move(event));
}
void Authenticator::logout(Susi::EventPtr event){
    std::string token = getTokenFromEvent(event);
    auto & user = usersByToken[token];
    user->token = "";
    usersByToken.erase(user->token);
    event->headers.push_back({"Event-Control","No-Consumer"});
    susi_->ack(std::move(event));
}


bool Authenticator::checkIfPayloadMatchesPattern(BSON::Value pattern, BSON::Value payload){
    if(pattern.getType() != payload.getType()){
        return false;
    }
    switch(pattern.getType()){
        case BSON::UNDEFINED:
        case BSON::INT32:
        case BSON::INT64:
        case BSON::DOUBLE:
        case BSON::BOOL:
        case BSON::DATETIME: {
            return pattern == payload;
        }
        case BSON::STRING:
        case BSON::BINARY: {
            std::regex r{pattern.getString()};
            return std::regex_match(payload.getString(),r);
        }
        case BSON::OBJECT: {
            for(auto & kv : pattern.getObject()){
                if(!checkIfPayloadMatchesPattern(kv.second,payload[kv.first])){
                    return false;
                }
            }
            return true;
        }
        case BSON::ARRAY: {
            if(pattern.size() != payload.size()){
                return false;
            }
            for(size_t i=0;i<pattern.size();i++){
                if(!checkIfPayloadMatchesPattern(pattern[i],payload[i])){
                    return false;
                }
            }
            return true;
        }
    }
    return false;
}

void Authenticator::registerGuard(Permission permission){
    susi_->registerProcessor(permission.pattern.topic,[permission,this](Susi::EventPtr event){
        if(checkIfPayloadMatchesPattern(permission.pattern.payload,event->payload)){
            std::string token = getTokenFromEvent(event);
            if(!usersByToken.count(token)){
                susi_->dismiss(std::move(event));
            }else{
                auto & u = usersByToken[token];
                for(auto & userRole : u->roles){
                    for(auto & permissionRole: permission.roles){
                        if(userRole == permissionRole){
                            susi_->ack(std::move(event));
                            return;
                        }
                    }
                }
                susi_->dismiss(std::move(event));
            }
        }
    });
}
