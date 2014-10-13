#include "CommandlineParser.h"

std::vector<std::string> & Debugger::Parser::split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

bool Debugger::Parser::set(std::string key, std::string value) {
	std::map<std::string,std::string>::iterator it;
	it = _knownCommandLineOptions.find(key);

	if(it != _knownCommandLineOptions.end()) {
		std::string key_found = it->second;
			
		if(_knownCommandLineType[key_found] == "multi") {
			if(_knownCommandLineValues[key_found] == "") 
				_knownCommandLineValues[key_found] = value;
			else 
				_knownCommandLineValues[key_found] = _knownCommandLineValues[key_found] + " " + value;
		} else {
			_knownCommandLineValues[key_found] = value;
		}

		return true;
	}

	return false;
}

std::string Debugger::Parser::getValueByKey(std::string key) {
	std::map<std::string,std::string>::iterator it;
	it = _knownCommandLineValues.find(key);

	if(it != _knownCommandLineValues.end()) {
		return it->second;		
	}

	return "";
}

void Debugger::Parser::registerCommandLineOption(std::string name, std::string key, std::string _default, std::string _type) {
	_knownCommandLineOptions[name] = key;
	_knownCommandLineValues[key]   = _default;
	_knownCommandLineType[key]     = _type; // "single or multi"
}

std::vector<std::string> Debugger::Parser::parseCommandLine(std::vector<std::string> argv) {
	
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


void Debugger::Parser::printParsedArgs() {

  for (std::map<std::string,std::string>::iterator it=_knownCommandLineValues.begin(); it!=_knownCommandLineValues.end(); ++it)
    std::cout << it->first << " => " << it->second <<std::endl;
}