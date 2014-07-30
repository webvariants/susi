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

TEST(Any, UNDEFINED) {

	// Contructor
	Any a = Any();
	EXPECT_EQ(Any::UNDEFINED,a.getType());
	EXPECT_TRUE(a.isNull());

	// Copy Contructor
	Any b = a;
	EXPECT_EQ(Any::UNDEFINED,b.getType());
	EXPECT_TRUE(b.isNull());

	// Move Contructor -> copy asignment operator
	Any c = std::move(a);
	EXPECT_TRUE(c.isNull());
}

TEST(Any, BOOL) {

	// Contructor
	Any a = Any(true);
	EXPECT_EQ(Any::BOOL,a.getType());
	EXPECT_TRUE(a.isBool());

	// Copy Contructor
	Any b = a;
	EXPECT_EQ(Any::BOOL,b.getType());
	EXPECT_TRUE(b.isBool());

	// Move Contructor -> copy asignment operator
	Any c = std::move(a);	
	EXPECT_TRUE(c.isBool());

	// Move Contructor
	Any d = false;
	EXPECT_EQ(Any::BOOL,d.getType());
	EXPECT_TRUE(d.isBool());

	bool e = d;
	EXPECT_FALSE(e);
}

TEST(Any, INTEGER) {
	
	// Contructor
	Any a = Any(10);
	EXPECT_EQ(Any::INTEGER,a.getType());
	EXPECT_TRUE(a.isInteger());

	// Copy Contructor
	Any b = a;
	EXPECT_EQ(Any::INTEGER,b.getType());
	EXPECT_TRUE(b.isInteger());

	// Move Contructor -> copy asignment operator
	Any c = std::move(a);	
	EXPECT_EQ(Any::INTEGER,c.getType());
	EXPECT_TRUE(c.isInteger());

	// Move Contructor
	Any d = 7;
	EXPECT_EQ(Any::INTEGER,d.getType());
	EXPECT_TRUE(d.isInteger());

	int e = d;
	EXPECT_EQ(7, e);
}

TEST(Any, DOUBLE) {
	
	// Contructor
	Any a = Any(1.5);
	EXPECT_EQ(Any::DOUBLE,a.getType());
	EXPECT_TRUE(a.isDouble());

	// Copy Contructor
	Any b = a;
	EXPECT_EQ(Any::DOUBLE,b.getType());
	EXPECT_TRUE(b.isDouble());

	// Move Contructor -> copy asignment operator
	Any c = std::move(a);	
	EXPECT_EQ(Any::DOUBLE,c.getType());
	EXPECT_TRUE(c.isDouble());

	// Move Contructor
	Any d = 7.6;
	EXPECT_EQ(Any::DOUBLE,d.getType());
	EXPECT_TRUE(d.isDouble());

	double e = d;
	EXPECT_EQ(7.6, e);
}

TEST(Any, STRING) {
	

	// Contructor
	Any a = Any("Foo");
	EXPECT_EQ(Any::STRING,a.getType());
	EXPECT_TRUE(a.isString());

	// Copy Contructor
	Any b = a;
	EXPECT_EQ(Any::STRING,b.getType());
	EXPECT_TRUE(b.isString());


	// Move Contructor -> copy asignment operator
	Any c = std::move(a);	
	EXPECT_EQ(Any::STRING,c.getType());
	EXPECT_TRUE(c.isString());


	// Move Contructor
	Any d = "Bar";
	EXPECT_EQ(Any::STRING,d.getType());
	EXPECT_TRUE(d.isString());

	std::string e = d;
	EXPECT_EQ("Bar", e);

}

TEST(Any, ARRAY) {
	
	// Contructor
	Any a = Any(std::deque<Any>{1,2,3,4});
	EXPECT_EQ(Any::ARRAY,a.getType());
	EXPECT_TRUE(a.isArray());
	EXPECT_EQ(4,a.size());

	// Copy Contructor
	Any b = a;
	EXPECT_EQ(Any::ARRAY,b.getType());
	EXPECT_TRUE(b.isArray());


	// Move Contructor -> copy asignment operator
	Any c = std::move(a);	
	EXPECT_EQ(Any::ARRAY,c.getType());
	EXPECT_TRUE(c.isArray());


	// Move Contructor
	Any d = std::deque<Any>{true, 5, 7.8,"FOO", Any(std::deque<Any>{"A",2})};
	EXPECT_EQ(Any::ARRAY,d.getType());
	EXPECT_TRUE(d.isArray());

	std::cout<<"TEST:"<<d.toString()<<std::endl;

	Any d1 = d[0];
	Any d2 = d[1];
	Any d3 = d[2];
	Any d4 = d[3];

	EXPECT_EQ(Any::BOOL,d1.getType());
	EXPECT_EQ(Any::INTEGER,d2.getType());
	EXPECT_EQ(Any::DOUBLE,d3.getType());
	EXPECT_EQ(Any::STRING,d4.getType());

	EXPECT_TRUE(d1.isBool());
	EXPECT_TRUE(d2.isInteger());
	EXPECT_TRUE(d3.isDouble());
	EXPECT_TRUE(d4.isString());

	bool       bool_d1 = d1;
	int        int_d2  = d2;
	double   double_d3 = d3;
	std::string str_d4 = d4;

	EXPECT_EQ(true , bool_d1);
	EXPECT_EQ(5    , int_d2);
	EXPECT_EQ(7.8  , double_d3);
	EXPECT_EQ("FOO", str_d4);

	// asignent
	std::deque<Any> e = d;

	Any e1 = e[0];
	Any e2 = e[1];
	Any e3 = e[2];
	Any e4 = e[3];

	EXPECT_EQ(Any::BOOL,e1.getType());
	EXPECT_EQ(Any::INTEGER,e2.getType());
	EXPECT_EQ(Any::DOUBLE,e3.getType());
	EXPECT_EQ(Any::STRING,e4.getType());

	EXPECT_TRUE(e1.isBool());
	EXPECT_TRUE(e2.isInteger());
	EXPECT_TRUE(e3.isDouble());
	EXPECT_TRUE(e4.isString());

	bool       bool_e1 = e1;
	int        int_e2  = e2;
	double   double_e3 = e3;
	std::string str_e4 = e4;

	EXPECT_EQ(true , bool_e1);
	EXPECT_EQ(5    , int_e2);
	EXPECT_EQ(7.8  , double_e3);
	EXPECT_EQ("FOO", str_e4);


	Any f = Any(std::deque<Any>{});
	Any f1 = "FOO";
	Any f2 = "BAR";
	Any f3 = "---";

	EXPECT_EQ(0,f.size());
	f.push_back(f1);
	f.push_back(f2);
	f.push_back(f3);
	EXPECT_EQ(3,f.size());

	Any f4 = f[0];
	std::string f4_str = f4;
	EXPECT_EQ("FOO",f4_str);

	Any f5 = f[2];
	std::string f5_str = f5;
	EXPECT_EQ("---",f5_str);

	f.push_front(f2);
	Any f6 = f[0];
	std::string f6_str = f6;
	EXPECT_EQ("BAR",f6_str);
	EXPECT_EQ(4,f.size());

	f.pop_front();
	EXPECT_EQ(3,f.size());

	Any f7 = f[0];
	std::string f7_str = f7;
	EXPECT_EQ("FOO",f7_str);

	f.pop_back();
	EXPECT_EQ(2,f.size());

	Any f8 = f[1];
	std::string f8_str = f8;
	EXPECT_EQ("BAR",f8_str);
}

TEST(Any, OBJECT) {

	// Contructor
	Any a = Any(std::map<std::string,Any>{ {"foo", "bar" } });
	EXPECT_EQ(Any::OBJECT,a.getType());
	EXPECT_TRUE(a.isObject());

	std::cout<<"TEST2:"<<a.toString()<<std::endl;

	// Copy Contructor
	Any b = a;
	EXPECT_EQ(Any::OBJECT,b.getType());
	EXPECT_TRUE(b.isObject());


	// Move Contructor -> copy asignment operator
	Any c = std::move(a);	
	EXPECT_EQ(Any::OBJECT,c.getType());
	EXPECT_TRUE(c.isObject());


	// Move Contructor
	Any d = std::map<std::string,Any>{ {"foo", "bar" } };
	EXPECT_EQ(Any::OBJECT,d.getType());
	EXPECT_TRUE(d.isObject());

	//std::map<std::string,Any> e = d;
	//EXPECT_EQ("Bar", e);

	/*
	Any o = std::map<std::string,Any>{ {"foo", "bar" } };
	EXPECT_EQ(Any::OBJECT,o.getType());
	std::map<std::string, Any> & o_ = o;
	EXPECT_EQ(o,o_);
	*/
}