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

std::pair<std::shared_ptr<Poco::Net::HTTPResponse>, std::string> Susi::HttpClient::get(std::string req) {
	std::shared_ptr<Poco::Net::HTTPResponse> res{new Poco::Net::HTTPResponse()};
	std::string body;
	try
	  {
	    // prepare session
	    std::string reqUri = _uri;
	    Poco::URI uri(reqUri.append(req));

	    std::cout<<"HOST:"<<uri.getHost()<<std::endl;
	    std::cout<<"PORT:"<<uri.getPort()<<std::endl;

	    Poco::Net::NameValueCollection cookies;
	    cookies.add("susisession", "httpclient_test");

	    Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());

	    // prepare path
	    std::string path(uri.getPathAndQuery());

	    std::cout<<"PATH:"<<path<<std::endl;
	    if (path.empty()) path = "/";

	    // send request
	    Poco::Net::HTTPRequest req(Poco::Net::HTTPRequest::HTTP_GET, path, Poco::Net::HTTPMessage::HTTP_1_1);
	    req.setCookies(cookies);
	    session.sendRequest(req);

	    // print response
	    std::istream &is = session.receiveResponse(*res);
	    // std::cout <<"STATUS:"<< res->getStatus() << " " << res->getReason() << std::endl;

	    Poco::StreamCopier::copyToString(is, body);

	    }
	  catch (Poco::Exception &ex)
	  {
	    std::cerr << ex.displayText() << std::endl;
	  }
	return std::make_pair(res, body);
}

std::pair<std::shared_ptr<Poco::Net::HTTPResponse>, std::string> Susi::HttpClient::post(std::string body, std::string uri_, std::vector<std::pair<std::string, std::string>> headers)  {
	if(uri_ == "") {
		uri_ = _uri;
	}
	Poco::URI uri(uri_);

	std::cout<<"HOST:"<<uri.getHost()<<std::endl;
	std::cout<<"PORT:"<<uri.getPort()<<std::endl;

	// prepare path
	std::string path(uri.getPath());
	if (path.empty()) path = "/";
	std::cout<<"PATH:"<<path<<std::endl;

	Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());

	Poco::Net::HTTPRequest req(Poco::Net::HTTPRequest::HTTP_POST, path, Poco::Net::HTTPMessage::HTTP_1_1);
	req.setContentType("application/x-www-form-urlencoded");
	req.setContentLength( body.length() );

	if(headers.size() > 0 && headers.size() < req.getFieldLimit()) {
		for(auto h : headers) {
			req.add(h.first, h.second);
		}
	}

	std::ostream & myout = session.sendRequest(req);
	myout << body;

	req.write(std::cout);

	// Receive the response.
	std::shared_ptr<Poco::Net::HTTPResponse> res{new Poco::Net::HTTPResponse()};
	std::string response_body;
	try {
		std::istream& rs = session.receiveResponse(*res);

		std::cout <<"STATUS:"<< res->getStatus() << " " << res->getReason() << std::endl;

		Poco::StreamCopier::copyToString(rs, response_body);
		std::cout << response_body << std::endl;

	} catch (Poco::Exception &ex) {
		std::cerr << ex.displayText() << std::endl;
	}

	return std::make_pair(res, response_body);
}

void Susi::HttpClient::connectWebSocket(std::string socket) {

}

void Susi::HttpClient::send(std::string data) {

}

std::string Susi::HttpClient::recv() {
	std::string result = "bla";
	return result;
}
