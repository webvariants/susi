#include "susi/SusiClient.h"
#include "bcrypt/BCrypt.hpp"
#include <regex>

class Authenticator {
public:
	Authenticator(std::string addr,short port, std::string key, std::string cert);
	void join();

protected:
	void login(Susi::EventPtr event);
	void logout(Susi::EventPtr event);

	void addUser(Susi::EventPtr event);
	void delUser(Susi::EventPtr event);
	void getUsers(Susi::EventPtr event);

	void addPermission(Susi::EventPtr event);
	void delPermission(Susi::EventPtr event);
	void getPermissions(Susi::EventPtr event);

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

	std::map<std::string,std::shared_ptr<User>> usersByName;
	std::map<std::string,std::shared_ptr<User>> usersByToken;
	std::map<std::string,Permission> permissionsByTopic;

	void addUser(std::shared_ptr<User> user);
	void addPermission(Permission permission);
	void load();
	void save();

	std::string generateToken();
	std::string getTokenFromEvent(const Susi::EventPtr & event);

	void registerGuard(Permission permission);

	bool checkIfPayloadMatchesPattern(BSON::Value pattern, BSON::Value payload);

	std::shared_ptr<Susi::SusiClient> susi_;
};
