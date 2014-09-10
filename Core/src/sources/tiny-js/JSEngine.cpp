#include "tiny-js/JSEngine.h"


Susi::TinyJSEngine::TinyJSEngine() : js(new CTinyJS()) {}

Susi::TinyJSEngine::TinyJSEngine(std::string filename) : js(new CTinyJS()) {
	runFile(filename);
}

std::string Susi::TinyJSEngine::runFile(std::string filename) {
	std::ifstream file(filename, std::ios::binary);
	std::string fileContents((std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>());
	return run(fileContents);
}

std::string Susi::TinyJSEngine::run(std::string source) {
	std::string result = "error";
	try{
		result = js->evaluate(source);
	}catch(const std::exception & e){
		std::cout<<"got error!"<<std::endl;
	}
	return result;
}