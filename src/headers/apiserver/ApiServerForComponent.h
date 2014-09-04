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

#ifndef __APISERVER__
#define __APISERVER__

#include <memory>
#include "events/global.h"
#include <functional>
#include <mutex>

namespace Susi {
namespace Api {

class ApiServerForComponent : ApiServer {
protected:
	std::shared_ptr<Susi::Events::ManagerComponent> eventManager;
	void handleRegisterConsumer(std::string & id, Susi::Util::Any & packet);
	void handleRegisterProcessor(std::string & id, Susi::Util::Any & packet);
	void handleUnregisterConsumer(std::string & id, Susi::Util::Any & packet);
	void handleUnregisterProcessor(std::string & id, Susi::Util::Any & packet);
	void handlePublish(std::string & id, Susi::Util::Any & packet);
	void handleAck(std::string & id, Susi::Util::Any & packet);

public:
	ApiServerForComponent(std::shared_ptr<Susi::Events::ManagerComponent> _eventManager) {
		eventManager = _eventManager;
	}
	void onClose(std::string & id);
};

}
}

#endif // __APISERVER__
