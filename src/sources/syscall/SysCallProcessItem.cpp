#include "syscall/SysCallProcessItem.h"

Susi::Syscall::ProcessItem::ProcessItem() {
	this->cmd  = "";
	this->args = "";
	this->background = false;
}

Susi::Syscall::ProcessItem::ProcessItem(std::string cmd, std::string args, bool background) {
	this->cmd  = cmd;
	this->args = args;
	this->background = background;
}

std::vector<std::string> & Susi::Syscall::ProcessItem::split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> Susi::Syscall::ProcessItem::getArgs(std::map<std::string, std::string> argsReplace) {
    std::vector<std::string> elems;
    std::vector<std::string> result;
    this->split(args, ' ', elems);

    for(size_t e=0; e<elems.size(); e++)
	{
		std::string elem = elems[e];

		if(elem[0] == '$') {
			std::string key = elem.substr(1,elem.size()+1);

			auto searchItem = argsReplace.find(key);

			if(searchItem != argsReplace.end()) {
				// Found
				result.push_back(searchItem->second);
			} else {
				// Not Found
				result.push_back(elem);
			}
		} else {
			// No arg
			result.push_back(elem);
		}
	}

    return result;
}