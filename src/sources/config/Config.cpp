/*
 * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 * 
 * @author: Thomas Krause (thomas.krause@webvariants.de)
 */

 #include "config/Config.h"

 Susi::Config::Config(std::string filename) {
 	Susi::IOController io;

 	std::string content = "";

 	try {
 		content = io.readFile(filename);
 	} catch(const std::exception & e){
		std::string msg = "Error reading Config File: " + filename;
		msg += e.what();
		throw std::runtime_error(msg);
	}

	try {
		_configVar = Susi::Util::Any::fromString(content);
	} catch(const std::exception & e){
		std::string msg = "file cant be parsed as json!";
		msg += e.what();
		throw std::runtime_error(msg);
	}

	if(_configVar.getType() != Susi::Util::Any::OBJECT) {
		throw std::runtime_error("file doesn't contain a (json) object");	
	}
 }

// used to set a value in the config object (should be used by parseCommandLine())
void Susi::Config::set(std::string key, Any value) {
	std::vector<std::string> elems;
	Susi::Util::Helpers::split(key, '.', elems);	
	auto * current = &_configVar;

	for(size_t e=0; e<elems.size()-1; e++){
		current = &(*current)[elems[e]];
		if((*current).isNull()){
			*current = Susi::Util::Any::Object{};
		}
	}
	(*current)[elems.back()] = value;
}

 // register a commandline option which will be recognized while parsing
void Susi::Config::registerCommandLineOption(std::string name, std::string key) {
	_knownCommandLineOptions[name] = key;
}

// parses the commandline
// loops though the args and check if it is in _knownCommandLineOptions
// if so, place it in the _configVar at the specified key
// should throw an error if unknown commandline options are supplied, but should parse everything else
void Susi::Config::parseCommandLine(int argc, char *argv[]) {
	std::map<std::string,std::string>::iterator it;

	if(argc < 3) {
		std::cout<<"not enough args!"<<std::endl;
	} else {
		for (int i = 1; i < argc; ++i) {
			std::string option = argv[i];
			if(option[0] == '-') {				
				it = _knownCommandLineOptions.find(option.substr(1));
				if(it != _knownCommandLineOptions.end()) {
					std::string key = it->second;
					if(i < (argc-1)) {
						std::string value = argv[(i+1)];						
						Susi::Util::Any v = Susi::Util::Any::fromString(value);								
						this->set(key, v);		
						Susi::Util::Any t = this->get(key);
					}
					
				} else {
					std::cout<<"command NOT found"<<std::endl;	
				}
			}
		}
	}
}

// get a config variable.
// keys are in this format: "foo.bar.baz"
// -> if we have {foo:{bar:{baz:123}}} get("foo.bar.baz") should return Any{123};
// should throw if key doesn't exist
Susi::Util::Any Susi::Config::get(std::string key) {
	std::vector<std::string> elems;

	if(key.length() == 0) {
		return _configVar;
	} else {
		Susi::Util::Helpers::split(key, '.', elems);

		Susi::Util::Any found = _configVar;
		Susi::Util::Any next  = _configVar;
		for(size_t e=0; e<elems.size(); e++)
		{
			std::string elem = elems[e];
			found = next[elem];
			next = found;
			if(found.getType() == Susi::Util::Any::UNDEFINED) {
				throw std::runtime_error("key doesn't exist!"+key);
			}
			
		}

		return found;
	}
}

// returns a help message which shows which options are available
std::string Susi::Config::getHelp() {
	std::string result;

	return "foo";
}