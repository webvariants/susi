#include "util/Glob.h"

Susi::Util::Glob::Glob(std::string pattern) {
	_pattern = pattern;
}

bool Susi::Util::Glob::match(std::string str) {

	const char *wild   = _pattern.c_str();
	const char *string = str.c_str();


	const char *cp = NULL, *mp = NULL;

	while ((*string) && (*wild != '*')) {
		if ((*wild != *string) && (*wild != '?')) {
		return 0;
		}
		wild++;
		string++;
	}

	while (*string) {
		if (*wild == '*') {
			if (!*++wild) {
				return 1;
			}
			mp = wild;
			cp = string+1;
		} else if ((*wild == *string) || (*wild == '?')) {
			wild++;
			string++;
		} else {
			wild = mp;
			string = cp++;
		}
	}

	while (*wild == '*') {
		wild++;
	}
	
	return !*wild;	
}

bool Susi::Util::Glob::isGlob(std::string pattern) {
	if(pattern.find("*") != std::string::npos ||
	   pattern.find("?") != std::string::npos ){
		return true;
	}	
	return false;
}