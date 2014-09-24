#include "util/Helpers.h"

std::vector<std::string> & Susi::Util::Helpers::split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

bool Susi::Util::Helpers::isInteger(const std::string & s)
{
   if(s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) return false ;

   char * p ;
   strtol(s.c_str(), &p, 10) ;

   return (*p == 0) ;
}

bool Susi::Util::Helpers::isDouble(const std::string & s) {
  std::istringstream ss(s);
  double d;
  return (ss >> d) && (ss >> std::ws).eof();
}
