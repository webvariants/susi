#include "leveldb/db.h"
#include "susi/SusiClient.h"
#include "susi/sha3.h"
#include <uuid/uuid.h>


class Authenticator {
public:
	Authenticator(std::string addr,short port, std::string key, std::string cert);
	void join();

protected:

	struct User {
		std::string name;
		std::string pwHash;
		std::string token;
		std::vector<std::string> roles;
	};

	struct Permission {
		Susi::Event pattern;
		std::vector<std::string> roles;
	};

	std::map<std::string,User> usersByName;
	std::map<std::string,User> usersByToken;
	std::map<std::string,Permission> permissionsByTopic;

	void addUser(User user);
	void addPermission(Permission permission);
	void load();
	void save();

	std::string generateToken();
	std::string getTokenFromEvent(const Susi::EventPtr & event);

	void login(Susi::EventPtr event);
	void logout(Susi::EventPtr event);
	void registerGuard(std::string topic);

	std::shared_ptr<Susi::SusiClient> susi_;
};
