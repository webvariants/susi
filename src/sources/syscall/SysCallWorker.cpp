#include "syscall/SysCallWorker.h"

Susi::Syscall::Worker::Worker(std::string returnAddr, std::string cmd, std::vector<std::string> args, bool bg) {
	this->_returnAddr = returnAddr;
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
		auto start_payload = Susi::Event::Payload({
			{"started", true}
		});	

		Susi::Event start_event(_returnAddr, start_payload);
		Susi::publish(start_event);
	}

	int rc = ph.wait();
	
	auto end_payload = Susi::Event::Payload({
		{"stdout", this->getPipeContent(ostr)},
		{"stderr", this->getPipeContent(estr)},
		{"return", rc}
	});

	Susi::Event end_event(_returnAddr, end_payload);
	Susi::publish(end_event);
}