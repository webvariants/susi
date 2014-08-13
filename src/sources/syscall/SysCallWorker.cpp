#include "syscall/SysCallWorker.h"

Susi::Syscall::Worker::Worker(std::string process_type, std::string cmd, std::vector<std::string> args, bool bg) {
	this->_process_type = process_type;
	this->_cmd = cmd;
	this->_args = args;
	this->_bg = bg;
}

std::string Susi::Syscall::Worker::getPipeContent(Poco::PipeInputStream & str) {
	std::string s;
	int c = str.get();

	while (c != -1) { 
		s += (char) c;
		c = str.get();
	}

	return s;
}

void Susi::Syscall::Worker::run() {
	Poco::Pipe outPipe;
	Poco::Pipe errPipe;

	Poco::ProcessHandle ph = Poco::Process::launch(_cmd, _args, 0, &outPipe, &errPipe);

	Poco::PipeInputStream ostr(outPipe);
	Poco::PipeInputStream estr(errPipe);


	if(_bg == true) {
		auto start_payload = Susi::Util::Any::Object{
			{"process_type" , _process_type },
			{"started", true}			
		};
		auto started_event = Susi::Events::createEvent("syscall::startedProcess");
		started_event->payload["result"] = start_payload;

		Susi::Events::publish(std::move(started_event));
	}

	int rc = ph.wait();

	Susi::Util::Any end_payload = Susi::Util::Any::Object{
		{"process_type" , _process_type },
		{"stdout", this->getPipeContent(ostr)},
		{"stderr", this->getPipeContent(estr)},
		{"return", rc}
	};

	//std::cout<<"Payload:"<<end_payload.toString()<<std::endl;

	auto end_event = Susi::Events::createEvent("syscall::endProcess");
	end_event->payload["result"] = end_payload;

	Susi::Events::publish(std::move(end_event));	
}