/*
 * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 * 
 * @author: Tino Rusch (tino.rusch@webvariants.de), Thomas Krause (thomas.krause@webvariants.de)
 */

#include "gtest/gtest.h"
#include "util/Any.h"

#include <iostream>

using namespace Susi::Util;

TEST(Any, BOOL) {
	
	Any b = true;
	EXPECT_EQ(Any::BOOL,b.getType());
	bool & b_ = b;
	EXPECT_EQ(true,b_);
}

TEST(Any, Contruct) {
	
	Any b = true;
	EXPECT_EQ(Any::BOOL,b.getType());
	bool & b_ = b;
	EXPECT_EQ(true,b_);

	Any i = 10;
	EXPECT_EQ(Any::INTEGER,i.getType());
	int & i_ = i;
	EXPECT_EQ(10,i_);

	Any d = 10.1;
	EXPECT_EQ(Any::DOUBLE,d.getType());
	double & d_ = d;
	EXPECT_EQ(10.1,d_);

	Any s = "AAA";
	EXPECT_EQ(Any::STRING,s.getType());
	std::string & s_ = s;
	EXPECT_EQ("AAA",s_);


	Any a = std::deque<Any>{1,2,3,4};
	EXPECT_EQ(Any::ARRAY,a.getType());
	std::deque<Any> & a_ = a;
	EXPECT_EQ(4 ,a_.size());

	Any posval = a[0];
	int foo = a[1];
	EXPECT_EQ(2,foo);
	std::cout<<"POSVAL:"<<(int&)(posval)<<"  "<<posval.isInteger()<<std::endl;

	
	//bool test = a[0];
	//std::cout<<"POSVAL2:"<<test<<std::endl;
	//EXPECT_EQ(1 ,v);
	

	/*
	Any o = std::map<std::string,Any>{ {"foo", "bar" } };
	EXPECT_EQ(Any::OBJECT,o.getType());
	std::map<std::string, Any> & o_ = o;
	EXPECT_EQ(o,o_);
	*/
}
