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

typedef std::map<std::string,Any> Map;
typedef std::deque<Any> Array;

void EXPECT_EQ_MAP(Map a, Map b){
	EXPECT_EQ(a.size(),b.size());
	for(auto & kv : a){
		const Any & other = b[kv.first];
		EXPECT_EQ(kv.second,other);
	}
};

void EXPECT_EQ_MAP(Array a, Array b){
	EXPECT_EQ(a.size(),b.size());
	for(size_t i=0;i<a.size();i++){
		const Any & m1 = a[i];
		const Any & m2 = b[i];
		EXPECT_EQ(m1,m2);
	}
};

TEST(Any, UndefinedConstructorsAndAssignments) {
	// Contructor
	Any a;
	EXPECT_EQ(Any::UNDEFINED,a.getType());
	EXPECT_TRUE(a.isNull());

	// Copy Constructor
	Any b{a};
	EXPECT_EQ(Any::UNDEFINED,b.getType());
	EXPECT_TRUE(b.isNull());

	// Move Constructor
	Any c;
	Any d{std::move(c)};
	EXPECT_EQ(Any::UNDEFINED,d.getType());
	EXPECT_TRUE(d.isNull());

	// Copy asignment operator
	Any e = a;
	EXPECT_EQ(Any::UNDEFINED,e.getType());
	EXPECT_TRUE(e.isNull());

	// Move asignment operator
	Any f = std::move(a);	
	EXPECT_EQ(Any::UNDEFINED,f.getType());
	EXPECT_TRUE(f.isNull());
}

TEST(Any, BoolConstructorsAndAssignments) {
	// Contructor
	Any a{true};
	EXPECT_EQ(Any::BOOL,a.getType());
	EXPECT_TRUE(a.isBool());
	EXPECT_EQ(true,static_cast<bool>(a));

	// Copy Constructor
	Any b{a};
	EXPECT_EQ(Any::BOOL,b.getType());
	EXPECT_TRUE(b.isBool());
	EXPECT_EQ(true,static_cast<bool>(b));
	
	// Move Constructor
	Any c{true};
	Any d{std::move(c)};
	EXPECT_EQ(Any::BOOL,d.getType());
	EXPECT_TRUE(d.isBool());
	EXPECT_EQ(true,static_cast<bool>(d));
	EXPECT_TRUE(c.isNull());

	// Copy asignment operator
	Any e = a;
	EXPECT_EQ(Any::BOOL,e.getType());
	EXPECT_TRUE(e.isBool());
	EXPECT_EQ(true,static_cast<bool>(e));
	
	// Move asignment operator
	Any f = std::move(a);	
	EXPECT_EQ(Any::BOOL,f.getType());
	EXPECT_TRUE(f.isBool());
	EXPECT_EQ(true,static_cast<bool>(f));
	EXPECT_TRUE(a.isNull());
}

TEST(Any, IntegerConstructorsAndAssignments) {
	// Contructor
	Any a{123};
	EXPECT_EQ(Any::INTEGER,a.getType());
	EXPECT_TRUE(a.isInteger());
	EXPECT_EQ(123,static_cast<int>(a));

	// Copy Constructor
	Any b{a};
	EXPECT_EQ(Any::INTEGER,b.getType());
	EXPECT_TRUE(b.isInteger());
	EXPECT_EQ(123,static_cast<int>(b));
	
	// Move Constructor
	Any c{123};
	Any d{std::move(c)};
	EXPECT_EQ(Any::INTEGER,d.getType());
	EXPECT_TRUE(d.isInteger());
	EXPECT_EQ(123,static_cast<int>(d));
	EXPECT_TRUE(c.isNull());

	// Copy asignment operator
	Any e = a;
	EXPECT_EQ(Any::INTEGER,e.getType());
	EXPECT_TRUE(e.isInteger());
	EXPECT_EQ(123,static_cast<int>(e));
	
	// Move asignment operator
	Any f = std::move(a);	
	EXPECT_EQ(Any::INTEGER,f.getType());
	EXPECT_TRUE(f.isInteger());
	EXPECT_EQ(123,static_cast<int>(f));
	EXPECT_TRUE(a.isNull());
}

TEST(Any, DoubleConstructorsAndAssignments) {
	// Contructor
	Any a{123.123};
	EXPECT_EQ(Any::DOUBLE,a.getType());
	EXPECT_TRUE(a.isDouble());
	EXPECT_EQ(123.123,static_cast<double>(a));

	// Copy Constructor
	Any b{a};
	EXPECT_EQ(Any::DOUBLE,b.getType());
	EXPECT_TRUE(b.isDouble());
	EXPECT_EQ(123.123,static_cast<double>(b));
	
	// Move Constructor
	Any c{123.123};
	Any d{std::move(c)};
	EXPECT_EQ(Any::DOUBLE,d.getType());
	EXPECT_TRUE(d.isDouble());
	EXPECT_EQ(123.123,static_cast<double>(d));
	EXPECT_TRUE(c.isNull());

	// Copy asignment operator
	Any e = a;
	EXPECT_EQ(Any::DOUBLE,e.getType());
	EXPECT_TRUE(e.isDouble());
	EXPECT_EQ(123.123,static_cast<double>(e));
	
	// Move asignment operator
	Any f = std::move(a);	
	EXPECT_EQ(Any::DOUBLE,f.getType());
	EXPECT_TRUE(f.isDouble());
	EXPECT_EQ(123.123,static_cast<double>(f));
	EXPECT_TRUE(a.isNull());
}

TEST(Any, StringConstructorsAndAssignments) {
	// Contructor
	Any a{"teststring"};
	EXPECT_EQ(Any::STRING,a.getType());
	EXPECT_TRUE(a.isString());
	EXPECT_EQ("teststring",static_cast<std::string>(a));

	// Copy Constructor
	Any b{a};
	EXPECT_EQ(Any::STRING,b.getType());
	EXPECT_TRUE(b.isString());
	EXPECT_EQ("teststring",static_cast<std::string>(b));
	
	// Move Constructor
	Any c{"teststring"};
	Any d{std::move(c)};
	EXPECT_EQ(Any::STRING,d.getType());
	EXPECT_TRUE(d.isString());
	EXPECT_EQ("teststring",static_cast<std::string>(d));
	EXPECT_TRUE(c.isNull());

	// Copy asignment operator
	Any e = a;
	EXPECT_EQ(Any::STRING,e.getType());
	EXPECT_TRUE(e.isString());
	EXPECT_EQ("teststring",static_cast<std::string>(e));
	
	// Move asignment operator
	Any f = std::move(a);	
	EXPECT_EQ(Any::STRING,f.getType());
	EXPECT_TRUE(f.isString());
	EXPECT_EQ("teststring",static_cast<std::string>(f));
	EXPECT_TRUE(a.isNull());
}

TEST(Any, ArrayConstructorsAndAssignments) {
	// Contructor
	Any a{Array{1,"foo",3.14}};
	EXPECT_EQ(Any::ARRAY,a.getType());
	EXPECT_TRUE(a.isArray());
	EXPECT_EQ_MAP(Array{1,"foo",3.14},static_cast<Array&>(a));

	// Copy Constructor
	Any b{a};
	EXPECT_EQ(Any::ARRAY,b.getType());
	EXPECT_TRUE(b.isArray());
	EXPECT_EQ_MAP(Array{1,"foo",3.14},static_cast<Array&>(b));
	
	// Move Constructor
	Any c{Array{1,"foo",3.14}};
	Any d{std::move(c)};
	EXPECT_EQ(Any::ARRAY,d.getType());
	EXPECT_TRUE(d.isArray());
	EXPECT_EQ_MAP(Array{1,"foo",3.14},static_cast<Array&>(d));
	EXPECT_TRUE(c.isNull());

	// Copy asignment operator
	Any e = a;
	EXPECT_EQ(Any::ARRAY,e.getType());
	EXPECT_TRUE(e.isArray());
	EXPECT_EQ_MAP(Array{1,"foo",3.14},static_cast<Array&>(e));
	
	// Move asignment operator
	Any f = std::move(a);	
	EXPECT_EQ(Any::ARRAY,f.getType());
	EXPECT_TRUE(f.isArray());
	EXPECT_EQ_MAP(Array{1,"foo",3.14},static_cast<Array&>(f));
	EXPECT_TRUE(a.isNull());
}

TEST(Any, ObjectConstructorsAndAssignments) {
	// Contructor
	Any a{Map{{"foo","bar"},{"baz",234}}};
	EXPECT_EQ(Any::OBJECT,a.getType());
	EXPECT_TRUE(a.isObject());
	EXPECT_EQ_MAP(Map{{"foo","bar"},{"baz",234}},static_cast<Map&>(a));

	// Copy Constructor
	Any b{a};
	EXPECT_EQ(Any::OBJECT,b.getType());
	EXPECT_TRUE(b.isObject());
	EXPECT_EQ_MAP(Map{{"foo","bar"},{"baz",234}},static_cast<Map&>(b));
	
	// Move Constructor
	Any c{Map{{"foo","bar"},{"baz",234}}};
	Any d{std::move(c)};
	EXPECT_EQ(Any::OBJECT,d.getType());
	EXPECT_TRUE(d.isObject());
	EXPECT_EQ_MAP(Map{{"foo","bar"},{"baz",234}},static_cast<Map&>(d));
	EXPECT_TRUE(c.isNull());

	// Copy asignment operator
	Any e = a;
	EXPECT_EQ(Any::OBJECT,e.getType());
	EXPECT_TRUE(e.isObject());
	EXPECT_EQ_MAP(Map{{"foo","bar"},{"baz",234}},static_cast<Map&>(e));
	
	// Move asignment operator
	Any f = std::move(a);	
	EXPECT_EQ(Any::OBJECT,f.getType());
	EXPECT_TRUE(f.isObject());
	EXPECT_EQ_MAP(Map{{"foo","bar"},{"baz",234}},static_cast<Map&>(f));
	EXPECT_TRUE(a.isNull());
}

TEST(Any,UndefinedConversion){
	Any a;
	
	EXPECT_THROW({
		bool v = a;
		if(v){};
	},Any::WrongTypeException);
	
	EXPECT_THROW({
		int v = a;
		if(v){};
	},Any::WrongTypeException);
	
	EXPECT_THROW({
		double v = a;
		if(v){};
	},Any::WrongTypeException);
	
	EXPECT_THROW({
		std::string v = a;
		if(v==""){};
	},Any::WrongTypeException);
	
	EXPECT_THROW({
		Array v = a;
		if(v.size()){};
	},Any::WrongTypeException);
	
	EXPECT_THROW({
		Map v = a;
		if(v.size()){};
	},Any::WrongTypeException);
}


TEST(Any,BoolConversion){
	Any a{true};
	
	EXPECT_NO_THROW({
		bool v = a;
		EXPECT_TRUE(v);
	});
	
	EXPECT_THROW({
		int v = a;
		if(v){};
	},Any::WrongTypeException);

	EXPECT_THROW({
		double v = a;
		EXPECT_EQ(23.23,v);
	},Any::WrongTypeException);
	
	EXPECT_THROW({
		std::string v = a;
		if(v==""){};
	},Any::WrongTypeException);
	
	EXPECT_THROW({
		Array v = a;
		if(v.size()){};
	},Any::WrongTypeException);
	
	EXPECT_THROW({
		Map v = a;
		if(v.size()){};
	},Any::WrongTypeException);
}

TEST(Any,IntegerConversion){
	Any a{23};
	
	EXPECT_THROW({
		bool v = a;
		if(v){};
	},Any::WrongTypeException);
	
	EXPECT_NO_THROW({
		int v = a;
		EXPECT_EQ(23,v);
	});
	
	EXPECT_THROW({
		double v = a;
		if(v){};
	},Any::WrongTypeException);
	
	EXPECT_THROW({
		std::string v = a;
		if(v==""){};
	},Any::WrongTypeException);
	
	EXPECT_THROW({
		Array v = a;
		if(v.size()){};
	},Any::WrongTypeException);
	
	EXPECT_THROW({
		Map v = a;
		if(v.size()){};
	},Any::WrongTypeException);
}

TEST(Any,DoubleConversion){
	Any a{23.23};
	
	EXPECT_THROW({
		bool v = a;
		if(v){};
	},Any::WrongTypeException);
	
	EXPECT_THROW({
		int v = a;
		if(v){};
	},Any::WrongTypeException);

	EXPECT_NO_THROW({
		double v = a;
		EXPECT_EQ(23.23,v);
	});
	
	EXPECT_THROW({
		std::string v = a;
		if(v==""){};
	},Any::WrongTypeException);
	
	EXPECT_THROW({
		Array v = a;
		if(v.size()){};
	},Any::WrongTypeException);
	
	EXPECT_THROW({
		Map v = a;
		if(v.size()){};
	},Any::WrongTypeException);
}

TEST(Any,StringConversion){
	Any a{"foobar"};
	
	EXPECT_THROW({
		bool v = a;
		if(v){};
	},Any::WrongTypeException);
	
	EXPECT_THROW({
		int v = a;
		if(v){};
	},Any::WrongTypeException);

	EXPECT_THROW({
		double v = a;
		if(v){};
	},Any::WrongTypeException);
	
	EXPECT_NO_THROW({
		std::string v = a;
		EXPECT_EQ("foobar",v);
	});
	
	EXPECT_THROW({
		Array v = a;
		if(v.size()){};
	},Any::WrongTypeException);
	
	EXPECT_THROW({
		Map v = a;
		if(v.size()){};
	},Any::WrongTypeException);
}

TEST(Any,ArrayConversion){
	Array base{1,"foo",1.23};
	Any a{base};
	
	EXPECT_THROW({
		bool v = a;
		if(v){};
	},Any::WrongTypeException);
	
	EXPECT_THROW({
		int v = a;
		if(v){};
	},Any::WrongTypeException);

	EXPECT_THROW({
		double v = a;
		if(v){};
	},Any::WrongTypeException);
	
	EXPECT_THROW({
		std::string v = a;
		if(v.size()){};
	},Any::WrongTypeException);

	EXPECT_NO_THROW({
		Array v = a;
		EXPECT_EQ_MAP(base,v);
	});
	
	EXPECT_THROW({
		Map v = a;
		if(v.size()){};
	},Any::WrongTypeException);
}

TEST(Any,ObjectConversion){
	Map base{{"foo","bar"}};
	Any a{base};
	
	EXPECT_THROW({
		bool v = a;
		if(v){};
	},Any::WrongTypeException);
	
	EXPECT_THROW({
		int v = a;
		if(v){};
	},Any::WrongTypeException);

	EXPECT_THROW({
		double v = a;
		if(v){};
	},Any::WrongTypeException);
	
	EXPECT_THROW({
		std::string v = a;
		if(v.size()){};
	},Any::WrongTypeException);

	EXPECT_THROW({
		Array v = a;
		if(v.size()){}
	},Any::WrongTypeException);
	
	EXPECT_NO_THROW({
		Map v = a;
		EXPECT_EQ_MAP(base,v);
	});
}

TEST(Any,UndefinedWrongAssignment){
	EXPECT_NO_THROW({
		Any a = true;
	});
	EXPECT_NO_THROW({
		Any a = 123;
	});
	EXPECT_NO_THROW({
		Any a = 123.45;
	});
	EXPECT_NO_THROW({
		Any a = "foobar";
	});
	EXPECT_NO_THROW({
		Any a = Array{};
	});
	EXPECT_NO_THROW({
		Any a = Map{};
	});
}

TEST(Any,BoolWrongAssignment){
	Any a{true};
	
	EXPECT_NO_THROW({
		a = false;
	});
	
	EXPECT_THROW({
		a = 123;
	},Any::WrongTypeException);
	
	EXPECT_THROW({
		a = 123.45;
	},Any::WrongTypeException);
	
	EXPECT_THROW({
		a = "foobar";
	},Any::WrongTypeException);
	
	EXPECT_THROW({
		a = Array{};
	},Any::WrongTypeException);

	EXPECT_THROW({
		a = Map{};
	},Any::WrongTypeException);
}

TEST(Any,IntegerWrongAssignment){
	Any a{23};
	
	EXPECT_THROW({
		a = true;
	},Any::WrongTypeException);
	
	EXPECT_NO_THROW({
		a = 123;
	});
	
	EXPECT_THROW({
		a = 123.45;
	},Any::WrongTypeException);
	
	EXPECT_THROW({
		a = "foobar";
	},Any::WrongTypeException);
	
	EXPECT_THROW({
		a = Array{};
	},Any::WrongTypeException);

	EXPECT_THROW({
		a = Map{};
	},Any::WrongTypeException);
}

TEST(Any,DoubleWrongAssignment){
	Any a{123.45};
	
	EXPECT_THROW({
		a = true;
	},Any::WrongTypeException);
	
	EXPECT_THROW({
		a = 123;
	},Any::WrongTypeException);
	
	EXPECT_NO_THROW({
		a = 123.45;
	});
	
	EXPECT_THROW({
		a = "foobar";
	},Any::WrongTypeException);
	
	EXPECT_THROW({
		a = Array{};
	},Any::WrongTypeException);

	EXPECT_THROW({
		a = Map{};
	},Any::WrongTypeException);
}

TEST(Any,StringWrongAssignment){
	Any a{"foobar"};
	
	EXPECT_THROW({
		a = true;
	},Any::WrongTypeException);
	
	EXPECT_THROW({
		a = 123;
	},Any::WrongTypeException);
	
	EXPECT_THROW({
		a = 123.45;
	},Any::WrongTypeException);
	
	EXPECT_NO_THROW({
		a = "this works!";
	});

	EXPECT_THROW({
		a = Array{};
	},Any::WrongTypeException);

	EXPECT_THROW({
		a = Map{};
	},Any::WrongTypeException);
}

TEST(Any,ArrayWrongAssignment){
	Any a{Array{}};
	
	EXPECT_THROW({
		a = true;
	},Any::WrongTypeException);
	
	EXPECT_THROW({
		a = 123;
	},Any::WrongTypeException);
	
	EXPECT_THROW({
		a = 123.45;
	},Any::WrongTypeException);
	
	EXPECT_THROW({
		a = "-.-";
	},Any::WrongTypeException);

	EXPECT_NO_THROW({
		a = Array{};
	});

	EXPECT_THROW({
		a = Map{};
	},Any::WrongTypeException);
}

TEST(Any,ObjectWrongAssignment){
	Any a{Map{}};
	
	EXPECT_THROW({
		a = true;
	},Any::WrongTypeException);
	
	EXPECT_THROW({
		a = 123;
	},Any::WrongTypeException);
	
	EXPECT_THROW({
		a = 123.45;
	},Any::WrongTypeException);
	
	EXPECT_THROW({
		a = "-.-";
	},Any::WrongTypeException);

	EXPECT_THROW({
		a = Array{};
	},Any::WrongTypeException);

	EXPECT_NO_THROW({
		a = Map{};
	});
}

TEST(Any,ObjectIndexOperators){
	Any a{Map{}};
	EXPECT_NO_THROW({
		a["foo"] = "bar";
	});
	EXPECT_EQ("bar",static_cast<std::string>(a["foo"]));
	EXPECT_THROW({
		a[0] = 123;
	},Any::WrongTypeException);
}

TEST(Any,ArrayIndexOperators){
	Any a{Array{}};
	EXPECT_THROW({
		a[0] = "foobar";
	},Any::WrongTypeException);
	a.push_back("foobar");
	EXPECT_NO_THROW({
		a[0] = "foobarbaz";
	});
	EXPECT_EQ("foobarbaz",static_cast<std::string>(a[0]));
	EXPECT_THROW({
		a["foo"] = "bar";
	},Any::WrongTypeException);
}

TEST(Any,ArrayHelper){
	Any a{Array{}};
	EXPECT_NO_THROW({
		a.push_back(0);
		a.push_back(1);
		a.push_back(2);
	});
	EXPECT_EQ(3,a.size());
	EXPECT_EQ(Any{0},a[0]);
	EXPECT_EQ(Any{1},a[1]);
	EXPECT_EQ(Any{2},a[2]);
	EXPECT_NO_THROW({
		a.pop_front();
		a.pop_back();
	});
	EXPECT_EQ(1,a.size());
	EXPECT_EQ(Any{1},a[0]);
}