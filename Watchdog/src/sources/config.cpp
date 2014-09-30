#include "config.h"

Config::Config() {
	registerCommandLineOption("kill_friendly", "killFriendly");
	registerCommandLineOption("restart_tries", "restartTrys");
	registerCommandLineOption("crash_restart", "restartChrashed");
}

std::vector<std::string> & Config::split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

bool Config::set(std::string key, std::string value) {
	std::map<std::string,std::string>::iterator it;
	it = _knownCommandLineOptions.find(key);

	if(it != _knownCommandLineOptions.end()) {
		std::string key_found = it->second;
		if(key_found == "killFriendly") {
			if(value == "" || value == "false") {
				kill_friendly = false;
			} else {
				kill_friendly = true;
			}			
			return true;
		}

		if(key_found == "restartTrys") {
			if(value == "") {
				restart_trys = 0;
			} else {
				int times = std::stoi(value);
				if(times >= 0) 
					restart_trys = times;
				else 
					restart_trys = -1;
			}			
			return true;
		}

		if(key_found == "restartChrashed") {
			if(value == "" || value == "false") {
				restart_crached = false;
			} else {
				restart_crached = true;
			}			
			return true;
		}
	}

	return false;
}

void Config::registerCommandLineOption(std::string name, std::string key) {
	_knownCommandLineOptions[name] = key;
}

std::vector<std::string> Config::parseCommandLine(std::vector<std::string> argv) {
	
	std::vector<std::string> filtered_args;
	std::map<std::string,std::string>::iterator it;

	int argc = argv.size();

	for (int i = 0; i < argc; ++i) {
		std::string option = argv[i];

		if(option[0] == '-') {
			if(option[1] == '-')option = option.substr(2);
			else option = option.substr(1);
			
			std::string v = "";
			std::string key = option;

			// find '='
			std::vector<std::string> elems;
			split(key, '=', elems);

			if(elems.size() == 2) {
				key = elems[0];
				v   = elems[1];

				if(set(key,v) == false) {
					filtered_args.push_back(argv[i]);
				}
			} else {
				if(i < (argc-1) && argv[i+1][0]!='-') {						
					v = argv[(i+1)];						
					if(set(key,v) == false) {
						filtered_args.push_back(argv[i]);
						filtered_args.push_back(argv[i+1]);
					}
					i++;
				}else{
					v = "";
					if(set(key,v) == false) {
						filtered_args.push_back(argv[i]);
					}
				}
			}
		} else {
			filtered_args.push_back(argv[i]);
		}
	}

	return filtered_args;	
}


bool Config::getExecutable(std::string path) {
	Poco::File f(this->getAbsPathFromString(path));

	if(f.exists()) {
		return f.canExecute();
	} else {
		return false;
	}
}

// helper function
Poco::Path Config::getAbsPathFromString(std::string path) {
	Poco::Path p(path);
	if(p.isRelative())
		p.makeAbsolute(this->base_path);
	return p;
}

void Config::printArgs(std::vector<std::string> argv_vec) {

  for (std::vector<std::string>::iterator it = argv_vec.begin(); it != argv_vec.end(); ++it)
    std::cout << ' ' << *it<< std::endl;
}