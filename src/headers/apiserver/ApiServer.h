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

#include "apiserver/ServerInterfaces.h"
#include <memory>

namespace Susi {
namespace Api {

class ApiServer {
protected:
	void handleRegisterConsumer(std::string & id, Susi::Util::Any & packet);
	void handleRegisterProcessor(std::string & id, Susi::Util::Any & packet);
	void handleUnregister(std::string & id, Susi::Util::Any & packet);
	void handlePublish(std::string & id, Susi::Util::Any & packet);
	
	virtual void onMessage(std::string & id, Susi::Util::Any & packet) override;
	virtual void send(std::string & id, Susi::Util::Any & packet) = 0;
	virtual void close(std::string & id) = 0;

public:
	virtual void run() = 0;
};

}
}


