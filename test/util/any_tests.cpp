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

}

TEST(Any, UNDEFINEDefewf) {
	
	// ctor
	//Any a = Any();
	
	// copy ctor
	//Any b = a;

	//Any & c = a;

	// move ctor ANY && copy asignment operator ANY
	//Any d = std::move(a);
	
	/*
	Any str = "HALLO";
	EXPECT_EQ(Any::STRING,str.getType());
	std::string s = str;
	EXPECT_EQ("HALLO",s);

	Any str2 = str;	
	std::string s2 = str2;
	EXPECT_EQ("HALLO",s2);

	Any str3 = std::move(str);
	std::string s3 = str3;
	EXPECT_EQ("HALLO",s3);

	std::string s4 = str;
	EXPECT_EQ("HALLO",s4);
	
	//std::string & s = str;
	//EXPECT_EQ("HALLO",s);
	*/

	//Any s1 = "Foo";
	//Any s2 = "Bar";

/*
	std::string  s3 = s1;
	std::string  s4 = s2;
	
	EXPECT_EQ("Foo",s3);
	EXPECT_EQ("Bar",s4);
*/
	/*
	s1 = std::move(s2);


	std::string  s5 = s1;
	//std::string  s6 = s2;

	EXPECT_EQ("Bar",s5);
//	EXPECT_EQ("Bar",s6);

	s2 = s1;
	std::string  s6 = s2;
	EXPECT_EQ("Bar",s6);
	/*
	Any d = "hallo";

	EXPECT_EQ(Any::UNDEFINED,a.getType());
	EXPECT_EQ(Any::UNDEFINED,b.getType());
	EXPECT_EQ(Any::UNDEFINED,c.getType());
	EXPECT_NE(Any::UNDEFINED,d.getType());
	*/
}

/*
TEST(Any, BOOL) {

	Any a = true;
	Any b = false;
	Any c = Any(true);
	Any d = Any(false);
	Any e = a;
	Any & f = b;
	bool g = a;
	bool h = b;

	EXPECT_TRUE(a.isBool());

	EXPECT_TRUE(a);
	EXPECT_FALSE(b);
	EXPECT_TRUE(c);
	EXPECT_FALSE(d);

	EXPECT_TRUE(e);
	EXPECT_FALSE(f);

	EXPECT_TRUE(g);
	EXPECT_FALSE(h);

	Any i = std::move(a);

	EXPECT_TRUE(i);
	EXPECT_TRUE(a);
}

TEST(Any, BOOLLLL) {
	
	Any a = Any(true);

	EXPECT_EQ(Any::BOOL,a.getType());
	EXPECT_TRUE(a.isBool());
	EXPECT_TRUE(a);

	Any b = false;
	EXPECT_EQ(Any::BOOL,b.getType());
	EXPECT_TRUE(b.isBool());
	EXPECT_FALSE(b);

	bool c = a;
	EXPECT_TRUE(c);
		 c = b;
	EXPECT_FALSE(b);

	a = b;
	EXPECT_FALSE(a);

	Any d = b;
	EXPECT_EQ(Any::BOOL,d.getType());
	EXPECT_TRUE(d.isBool());
	EXPECT_FALSE(d);

	Any & e = a;
	EXPECT_EQ(Any::BOOL,e.getType());
	EXPECT_FALSE(e);
	e = true;
	EXPECT_TRUE(e);

	Any f = std::move(a); 
	EXPECT_EQ(Any::BOOL,f.getType());
	EXPECT_TRUE(f.isBool());
	//EXPECT_FALSE(f);


	Any b = true;
	EXPECT_EQ(Any::BOOL,b.getType());

	Any b_any_copy = b;
	EXPECT_TRUE(b_any_copy.isBool());

	bool b_copy = b;
	EXPECT_TRUE(b_copy);
	EXPECT_EQ(1,b_copy);

	bool & b_referenz = b;
	EXPECT_EQ(true,b_referenz);

}

TEST(Any, Contruct) {


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
	

	
	Any o = std::map<std::string,Any>{ {"foo", "bar" } };
	EXPECT_EQ(Any::OBJECT,o.getType());
	std::map<std::string, Any> & o_ = o;
	EXPECT_EQ(o,o_);
	
}

*/