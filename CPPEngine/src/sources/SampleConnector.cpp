#include "SampleConnector.h"


/**
 * This is a little different. Here we want to get an integer representing the current time
 */
void SampleNamespace::SampleConnector::handleGetTime(Susi::Events::EventPtr event){
	struct AnswerCallback {
		AnswerCallback(Susi::Events::EventPtr event) : request{std::move(event)}{}
		AnswerCallback(AnswerCallback & other){std::swap(request,other.request);}
		AnswerCallback(AnswerCallback && other){std::swap(request,other.request);}
		Susi::Events::EventPtr request;
		void operator()(Susi::Events::SharedEventPtr event){
			std::cout<<"in gettime callback: "<<event->toString()<<std::endl;
			request->payload["time"] = std::stol(event->payload["stdout"]);
		}
	};
	AnswerCallback cb{std::move(event)};
	auto evt = createEvent("syscall::exec");
	evt->payload["cmd"] = "getTime";
	publish(std::move(evt),cb);
}
