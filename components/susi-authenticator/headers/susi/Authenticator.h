#include "susi/SusiClient.h"
#include "bcrypt/BCrypt.hpp"
#include <random>
#include <regex>
#include <fstream>

namespace Susi {
	class Authenticator {
	public:
		Authenticator(Susi::SusiClient & susi, BSON::Value & config);
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
			std::string id;
			Susi::Event pattern;
			std::vector<std::string> roles;
		};

		std::map<std::string,std::shared_ptr<User>> usersByName;
		std::map<std::string,std::shared_ptr<User>> usersByToken;
		std::map<std::string,std::map<std::string,Permission>> permissionsByTopic;

		void addUser(std::shared_ptr<User> user);
		void addPermission(Permission permission);
		void loadFromFile();
		void saveToFile();

		void setupDefaults();

		std::string generateToken();
		std::string getTokenFromEvent(const Susi::EventPtr & event);

		void registerGuard(Permission permission);

		bool checkIfPayloadMatchesPattern(const BSON::Value & pattern, const BSON::Value & payload);

		BSON::Value permissionsToBSON();
		bool BSONToPermissions(BSON::Value permissions);

		Susi::SusiClient & susi_;
		BSON::Value & config_;
	};
}
