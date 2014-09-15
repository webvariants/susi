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

#include "TinyJSEngine.h"

//Constructor
//Susi::JS::Tiny::Engine::Engine(std::string addr) : susi_client{addr} {}
Susi::JS::Tiny::Engine::Engine(std::string addr){}

void Susi::JS::Tiny::Engine::run(std::string code) {
	CTinyJS tiny;
  	registerFunctions(&tiny);
  	registerMathFunctions(&tiny);

  	const char * buffer = code.c_str();
  	
  	//tiny.setVariable("XXX", "45");
  	tiny.root->addChild("result", new CScriptVar("0",SCRIPTVAR_INTEGER));
  
  	try {
    	tiny.execute(buffer);
  	} catch (CScriptException *e) {
    	printf("ERROR: %s\n", e->text.c_str());
  	}

  	bool pass = tiny.root->getParameter("result")->getBool();
  	if(pass) {
  		std::cout<<"Passed"<<std::endl;
  	} else {
  		std::cout<<"Failed"<<std::endl;
  	}
}