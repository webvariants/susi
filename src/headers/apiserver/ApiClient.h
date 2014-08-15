/*
 * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 * 
 * @author: Tino Rusch (tino.rusch@webvariants.de)
 */

#ifndef __APICLIENT__
#define __APICLIENT__

#include "apiserver/BasicApiClient.h"
#include "events/Manager.h"

namespace Susi {
namespace Api {

class ApiClient : protected BasicApiClient, protected Susi::Events::Manager {
public:
	ApiClient(std::string addr) : BasicApiClient{addr} {}
	void close(){
		BasicApiClient::close();
	}
	virtual ~ApiClient(){
		close();
	}
	void publish(Susi::Events::EventPtr event, Susi::Events::Consumer finishCallback = Susi::Events::Consumer{});
	long subscribe(std::string topic, Susi::Events::Processor processor);
	long subscribe(std::string topic, Susi::Events::Consumer consumer);
	void ack(Susi::Events::EventPtr event){
		Susi::Events::Manager::ack(std::move(event));
	};
protected:
	virtual void onConsumerEvent(Susi::Events::Event & event) override;
	virtual void onProcessorEvent(Susi::Events::Event & event) override;
	virtual void onAck(Susi::Events::Event & event) override;

	virtual void onConnect() override {};
	virtual void onClose() override {};

	std::map<long,Susi::Events::Consumer> finishCallbacks;

};

}
}

#endif // __APICLIENT__
