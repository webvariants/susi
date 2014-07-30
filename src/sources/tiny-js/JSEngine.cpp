#include "tiny-js/JSEngine.h"


Susi::JSEngine::JSEngine() : js(new CTinyJS()) {}

Susi::JSEngine::JSEngine(std::string filename) : js(new CTinyJS()) {
	runFile(filename);
}

std::string Susi::JSEngine::runFile(std::string filename) {
	std::ifstream file(filename, std::ios::binary);
	std::string fileContents((std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>());
	return run(fileContents);
}

std::string Susi::JSEngine::run(std::string source) {
	std::string result = "error";
	try{
		result = js->evaluate(source);
	}catch(const std::exception & e){
		std::cout<<"got error!"<<std::endl;
	}
	return result;
}