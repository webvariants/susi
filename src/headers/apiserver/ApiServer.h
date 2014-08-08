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

#include <memory>
#include "apiserver/JSONServer.h"

namespace Susi {
namespace Api {

class ApiServer : JSONServer {
protected:
	
	virtual void onMessage(std::string & id, Susi::Util::Any & packet) override;
	
	void handleRegisterConsumer(std::string & id, Susi::Util::Any & packet);
	void handleRegisterProcessor(std::string & id, Susi::Util::Any & packet);
	void handleUnregister(std::string & id, Susi::Util::Any & packet);
	void handlePublish(std::string & id, Susi::Util::Any & packet);
	
};

}
}


