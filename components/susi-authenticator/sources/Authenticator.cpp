#include "susi/Authenticator.h"

Authenticator::Authenticator(std::string addr,short port, std::string key, std::string cert) :
  susi_{new Susi::SusiClient{addr,port,key,cert}} {
    load();
    susi_->registerProcessor("authenticator::login",[this](Susi::EventPtr event){
        login(std::move(event));
    });
    susi_->registerProcessor("authenticator::logout",[this](Susi::EventPtr event){
        logout(std::move(event));
    });
    susi_->registerProcessor("authenticator::users::add",[this](Susi::EventPtr event){
        addUser(std::move(event));
    });
    susi_->registerProcessor("authenticator::users::delete",[this](Susi::EventPtr event){
        delUser(std::move(event));
    });
    susi_->registerProcessor("authenticator::users::get",[this](Susi::EventPtr event){
        getUsers(std::move(event));
    });
    susi_->registerProcessor("authenticator::permissions::add",[this](Susi::EventPtr event){
        addPermission(std::move(event));
    });
    susi_->registerProcessor("authenticator::permissions::delete",[this](Susi::EventPtr event){
        delPermission(std::move(event));
    });
    susi_->registerProcessor("authenticator::permissions::get",[this](Susi::EventPtr event){
        getPermissions(std::move(event));
    });
}

void Authenticator::login(Susi::EventPtr event){
    auto name = event->payload["username"].getString();
    // check username and password
    if(usersByName.count(name) == 0){
        throw std::runtime_error{"no such user"};
    }
    auto & user = usersByName[name];
    if(!BCrypt::validatePassword(event->payload["password"].getString(),user->pwHash)){
        std::this_thread::sleep_for(std::chrono::milliseconds{100}); //this is agains brute forcers
        throw std::runtime_error{"wrong password"};
    }
    user->token = generateToken();
    event->payload["token"] = user->token;
    usersByToken[user->token] = user;

    // add all topics which require a valid token
    // (tokens will not be deleted if sended to others than this topics)
    BSON::Array topics;
    for(auto & kv : permissionsByTopic){
        topics.push_back(kv.first);
    }
    event->payload["topics"] = topics;

    // prevent arbitary consumers from reading sensitive data
    event->headers.push_back({"Event-Control","No-Consumer"});
    event->headers.push_back({"Event-Control","No-Processor"});
    susi_->ack(std::move(event));
}

void Authenticator::logout(Susi::EventPtr event){
    std::string token = getTokenFromEvent(event);
    auto & user = usersByToken[token];
    user->token = "";
    usersByToken.erase(user->token);
    susi_->ack(std::move(event));
}

void Authenticator::addUser(Susi::EventPtr event){
    std::string username = event->payload["username"];
    std::string password = event->payload["password"];
    auto roles = event->payload["roles"];
    auto user = std::make_shared<User>();
    user->name = username;
    user->pwHash = BCrypt::generateHash(password);
    for(size_t i=0;i<roles.size();i++){
        user->roles.emplace_back(roles[i].getString());
    }
    addUser(user);
    event->payload = true;
}

void Authenticator::delUser(Susi::EventPtr event){
    std::string username = event->payload["username"];
    auto user = usersByName[username];
    usersByName.erase(username);
    usersByToken.erase(user->token);
    saveUsers();
    event->payload = true;
}

void Authenticator::getUsers(Susi::EventPtr event){
    BSON::Array resultData;
    for(auto & kv : usersByName){
        auto & user = kv.second;
        resultData.push_back(BSON::Object{
            {"username",user->name},
            {"roles",BSON::Array{user->roles.begin(),user->roles.end()}}
        });
    }
    event->headers.push_back({"Event-Control","No-Consumer"});
    event->headers.push_back({"Event-Control","No-Processor"});
    event->payload = resultData;
}

void Authenticator::addPermission(Susi::EventPtr event){
    auto & pattern = event->payload["pattern"];
    auto roles = event->payload["roles"];
    std::string topic = pattern["topic"].getString();
    BSON::Value & payload = pattern["payload"];
    Susi::Event evt;
    evt.topic = topic;
    evt.payload = payload;
    Permission permission;
    permission.pattern = evt;
    permission.id = generateToken();
    std::vector<std::string> roleArray;
    for(size_t i=0;i<roles.size();i++){
        roleArray.emplace_back(roles[i].getString());
    }
    permission.roles = roleArray;
    addPermission(permission);
    event->payload = BSON::Object{
        {"id",permission.id}
    };
}

void Authenticator::delPermission(Susi::EventPtr event){
    std::string id = event->payload["id"].getString();
    for(auto & kv : permissionsByTopic){
        for(auto & idAndPerm : kv.second){
            if(idAndPerm.first == id){
                kv.second.erase(id);
                savePermissions();
                event->payload["success"] = true;
                return;
            }
        }
    }
    event->payload["success"] = false;
}

void Authenticator::getPermissions(Susi::EventPtr event){
    event->payload = permissionsToBSON();
    event->headers.push_back({"Event-Control","No-Consumer"});
    event->headers.push_back({"Event-Control","No-Processor"});
}

void Authenticator::addUser(std::shared_ptr<User> user){
    usersByName[user->name] = user;
    saveUsers();
}

void Authenticator::addPermission(Permission permission){
    permissionsByTopic[permission.pattern.topic][permission.id] = permission;
    registerGuard(permission);
    savePermissions();
}

BSON::Value Authenticator::permissionsToBSON(){
    BSON::Object permissionObject;
    for(auto & kv : permissionsByTopic){
        auto & permissions = kv.second;
        for(auto & idAndPerm : permissions){
            auto & permission = idAndPerm.second;
            permissionObject[permission.id] = BSON::Object{
                {"pattern",permission.pattern.toAny()},
                {"roles",BSON::Array{
                    permission.roles.begin(),
                    permission.roles.end()
                }}
            };
        }
    }
    return permissionObject;
}

bool Authenticator::BSONToPermissions(BSON::Value permissions){
    if(!permissions.isObject()){
        return false;
    }
    for(auto & kv : permissions.getObject()){
        auto & id = kv.first;
        auto & permission = kv.second;
        Permission p = {
            .id = id,
            .pattern = Susi::Event{permission["pattern"]}
        };
        for(size_t j=0;j<permission["roles"].size();j++){
            p.roles.push_back(permission["roles"][j].getString());
        }
        addPermission(p);
    }
    return true;
}

void Authenticator::loadUsers(){
    auto userRequestEvent = susi_->createEvent("state::get");
    userRequestEvent->payload["key"] = "authenticator::users";
    susi_->publish(std::move(userRequestEvent),[this](Susi::SharedEventPtr event){
        usersByName.clear();
        usersByToken.clear();
        auto & users = event->payload["value"];
        if(!users.isArray() || users.size()==0){
            setupDefaults();
        }else for(size_t i=0;i<users.size();i++){
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
}

void Authenticator::loadPermissions(){
    auto permissionRequestEvent = susi_->createEvent("state::get");
    permissionRequestEvent->payload["key"] = "authenticator::permissions";
    susi_->publish(std::move(permissionRequestEvent),[this](Susi::SharedEventPtr event){
        permissionsByTopic.clear();
        auto & permissions = event->payload["value"];
        BSONToPermissions(permissions);
    });
}


void Authenticator::load(){
    loadUsers();
    loadPermissions();
}


void Authenticator::saveUsers(){
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
}
void Authenticator::savePermissions(){
    BSON::Value permissions = permissionsToBSON();
    auto permissionSaveEvent = susi_->createEvent("state::put");
    permissionSaveEvent->payload["key"] = "authenticator::permissions";
    permissionSaveEvent->payload["value"] = permissions;
    susi_->publish(std::move(permissionSaveEvent));
}

void Authenticator::save(){
    saveUsers();
    savePermissions();
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
    for(auto & header : event->headers){
        if(header.first == "User-Token"){
            std::string token = header.second;
            header.second = "";
            return token;
        }
    }
    return "";
}

bool Authenticator::checkIfPayloadMatchesPattern(const BSON::Value & pattern, const BSON::Value & payload){
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
        std::string token = getTokenFromEvent(event);
        if(checkIfPayloadMatchesPattern(permission.pattern.payload,event->payload)){
            if(!usersByToken.count(token)){
                event->headers.push_back({"Error","Unauthenticated"});
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
                event->headers.push_back({"Error","Unauthenticated"});
                susi_->dismiss(std::move(event));
            }
        }
    });
}

void Authenticator::setupDefaults(){
    auto user = std::make_shared<User>();
    user->name = "root";
    user->pwHash = BCrypt::generateHash("toor");
    user->roles.push_back("admin");
    addUser(user);
    BSON::Value pattern = BSON::Object{
        {"topic","authenticator::(users|permissions)::.*"}
    };
    auto permission = Permission{
        .id = generateToken(),
        .pattern = Susi::Event{pattern},
        .roles = std::vector<std::string>{"admin"}
    };
    addPermission(permission);
}
