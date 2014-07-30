#include "util/TestHelper.h"

static const char alphanum[] = "0123456789!@#$%^&*ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
int stringLength = sizeof(alphanum) - 1;

std::string Susi::Util::__createRandomString(std::size_t len) {
	
	char buffer[len];

	for (int i = 0; i < (int)len; i++ ){ 
       buffer[i] =  alphanum[rand() % stringLength];
    }

    return std::string(buffer,len);
}