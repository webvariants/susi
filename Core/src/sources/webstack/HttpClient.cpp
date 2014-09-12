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

#include "webstack/HttpClient.h"


Susi::HttpClient::HttpClient(std::string uri) {
	_uri = uri;	
}
			
std::string Susi::HttpClient::get(std::string req) {
	std::string content;
	try
	  {
	    // prepare session
	    std::string reqUri = _uri;
	    Poco::URI uri(reqUri.append(req));

	    std::cout<<"HOST:"<<uri.getHost()<<std::endl;
	    std::cout<<"PORT:"<<uri.getPort()<<std::endl;

	    Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());

	    // prepare path
	    std::string path(uri.getPathAndQuery());

	    std::cout<<"PATH:"<<path<<std::endl;
	    if (path.empty()) path = "/";

	    // send request
	    Poco::Net::HTTPRequest req(Poco::Net::HTTPRequest::HTTP_GET, path, Poco::Net::HTTPMessage::HTTP_1_1);
	    session.sendRequest(req);

	    // get response
	    Poco::Net::HTTPResponse res;	    

	    // print response
	    std::istream &is = session.receiveResponse(res);

	    std::cout <<"STATUS:"<< res.getStatus() << " " << res.getReason() << std::endl;

	    //Poco::StreamCopier::copyStream(is, content);
	    Poco::StreamCopier::copyToString(is, content);
	    
	    std::cout<<"Content:"<<content<<std::endl;
	  }
	  catch (Poco::Exception &ex)
	  {
	    std::cerr << ex.displayText() << std::endl;
	    return "error";
	  }
	return content;
}

std::string Susi::HttpClient::post(std::string formular) {
	
	 std::string reqUri = _uri;
	 Poco::URI uri(reqUri.append(formular));

	 std::cout<<"HOST:"<<uri.getHost()<<std::endl;
	 std::cout<<"PORT:"<<uri.getPort()<<std::endl;

	 Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());

	 // prepare path
	 std::string path(uri.getPath());

	 std::cout<<"PATH:"<<path<<std::endl;
	 if (path.empty()) path = "/";

	 Poco::Net::HTTPRequest req(Poco::Net::HTTPRequest::HTTP_POST, path, Poco::Net::HTTPMessage::HTTP_1_1);
	 
	 req.setContentType("application/x-www-form-urlencoded");
	 
	 Poco::Net::HTMLForm form;
	 
	 form.add("action", "uploadVideoComplete");	 
	 form.add("checkOauth", "0dac14fbc490e2def12110fd215c1c42");


	 //form.add("", "");
	 // Send the request.
	 form.prepareSubmit(req);
	 std::ostream& ostr = session.sendRequest(req);
	 form.write(ostr);

	 try {
		 // Receive the response.
		 Poco::Net::HTTPResponse res;
		 std::istream& rs = session.receiveResponse(res);

		 std::cout <<"STATUS:"<< res.getStatus() << " " << res.getReason() << std::endl;

		 std::string responseText;
		 Poco::StreamCopier copier;
		 copier.copyToString(rs, responseText);
		 std::cout << responseText << std::endl;

	  } catch (Poco::Exception &ex) {
	    std::cerr << ex.displayText() << std::endl;
	    return "error";
	  }

	return "OK";
}

void Susi::HttpClient::connectWebSocket(std::string socket) {

}

void Susi::HttpClient::send(std::string data) {

}

std::string Susi::HttpClient::recv() {
	std::string result = "bla";
	return result;
}