/*
 * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 * 
 * @author: Thomas Krause (thomas.krause@webvariants.de)
 */

#ifndef __FORM_REQUEST_HANDLER__
#define __FORM_REQUEST_HANDLER__

#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Util/Application.h"

#include "Poco/Net/HTMLForm.h"
#include "Poco/Net/NameValueCollection.h"
/*
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/File.h"
#include "Poco/FileStream.h"
#include "Poco/StreamCopier.h"
*/

#include "logger/Logger.h"

#include "webstack/MyPartHandler.h"

namespace Susi {

	class FormRequestHandler: public Poco::Net::HTTPRequestHandler {
		//private:
			//Poco::File _rootDirectory;
		public:
			FormRequestHandler();
			//FormRequestHandler(std::string rootDirectory) :
	  		
    		virtual void handleRequest(Poco::Net::HTTPServerRequest& request,
            Poco::Net::HTTPServerResponse& response) override;
};

}

#endif // __FORM_REQUEST_HANDLER__
