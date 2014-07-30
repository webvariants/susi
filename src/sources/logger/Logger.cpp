#include "logger/Logger.h"
#include "world/World.h"

void Susi::log(std::string msg){
	if(world.logger.get() != nullptr) world.logger->log(msg);
}
void Susi::info(std::string msg){
	if(world.logger.get() != nullptr) world.logger->info(msg);
}
void Susi::warn(std::string msg){
	if(world.logger.get() != nullptr) world.logger->warn(msg);
}
void Susi::debug(std::string msg){
	if(world.logger.get() != nullptr) world.logger->debug(msg);
}
void Susi::error(std::string msg){
	if(world.logger.get() != nullptr) world.logger->error(msg);
}
void Susi::setLogLevel(unsigned char level){
	if(world.logger.get() != nullptr) world.logger->setLevel(level);
}