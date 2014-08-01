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

void EXPECT_EQ_OBJECT(Any::Object a, Any::Object b){
	EXPECT_EQ(a.size(),b.size());
	for(auto & kv : a){
		const Any & other = b[kv.first];
		EXPECT_EQ(kv.second,other);
	}
};

void EXPECT_EQ_ARRAY(Any::Array a, Any::Array b){
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
	Any a{Any::Array{1,"foo",3.14}};
	EXPECT_EQ(Any::ARRAY,a.getType());
	EXPECT_TRUE(a.isArray());
	EXPECT_EQ_ARRAY(Any::Array{1,"foo",3.14},static_cast<Any::Array&>(a));

	// Copy Constructor
	Any b{a};
	EXPECT_EQ(Any::ARRAY,b.getType());
	EXPECT_TRUE(b.isArray());
	EXPECT_EQ_ARRAY(Any::Array{1,"foo",3.14},static_cast<Any::Array&>(b));
	
	// Move Constructor
	Any c{Any::Array{1,"foo",3.14}};
	Any d{std::move(c)};
	EXPECT_EQ(Any::ARRAY,d.getType());
	EXPECT_TRUE(d.isArray());
	EXPECT_EQ_ARRAY(Any::Array{1,"foo",3.14},static_cast<Any::Array&>(d));
	EXPECT_TRUE(c.isNull());

	// Copy asignment operator
	Any e = a;
	EXPECT_EQ(Any::ARRAY,e.getType());
	EXPECT_TRUE(e.isArray());
	EXPECT_EQ_ARRAY(Any::Array{1,"foo",3.14},static_cast<Any::Array&>(e));
	
	// Move asignment operator
	Any f = std::move(a);	
	EXPECT_EQ(Any::ARRAY,f.getType());
	EXPECT_TRUE(f.isArray());
	EXPECT_EQ_ARRAY(Any::Array{1,"foo",3.14},static_cast<Any::Array&>(f));
	EXPECT_TRUE(a.isNull());

}

TEST(Any, ObjectConstructorsAndAssignments) {
	// Contructor
	Any a{Any::Object{{"foo","bar"},{"baz",234}}};
	EXPECT_EQ(Any::OBJECT,a.getType());
	EXPECT_TRUE(a.isObject());
	EXPECT_EQ_OBJECT(Any::Object{{"foo","bar"},{"baz",234}},static_cast<Any::Object&>(a));


	// Copy Constructor
	Any b{a};
	EXPECT_EQ(Any::OBJECT,b.getType());
	EXPECT_TRUE(b.isObject());
	EXPECT_EQ_OBJECT(Any::Object{{"foo","bar"},{"baz",234}},static_cast<Any::Object&>(b));
	
	// Move Constructor
	Any c{Any::Object{{"foo","bar"},{"baz",234}}};
	Any d{std::move(c)};
	EXPECT_EQ(Any::OBJECT,d.getType());
	EXPECT_TRUE(d.isObject());
	EXPECT_EQ_OBJECT(Any::Object{{"foo","bar"},{"baz",234}},static_cast<Any::Object&>(d));
	EXPECT_TRUE(c.isNull());

	// Copy asignment operator
	Any e = a;
	EXPECT_EQ(Any::OBJECT,e.getType());
	EXPECT_TRUE(e.isObject());
	EXPECT_EQ_OBJECT(Any::Object{{"foo","bar"},{"baz",234}},static_cast<Any::Object&>(e));
	
	// Move asignment operator
	Any f = std::move(a);	
	EXPECT_EQ(Any::OBJECT,f.getType());
	EXPECT_TRUE(f.isObject());
	EXPECT_EQ_OBJECT(Any::Object{{"foo","bar"},{"baz",234}},static_cast<Any::Object&>(f));
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
		Any::Array v = a;
		if(v.size()){};
	},Any::WrongTypeException);
	
	EXPECT_THROW({
		Any::Object v = a;
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
		Any::Array v = a;
		if(v.size()){};
	},Any::WrongTypeException);
	
	EXPECT_THROW({
		Any::Object v = a;
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
		Any::Array v = a;
		if(v.size()){};
	},Any::WrongTypeException);
	
	EXPECT_THROW({
		Any::Object v = a;
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
		Any::Array v = a;
		if(v.size()){};
	},Any::WrongTypeException);
	
	EXPECT_THROW({
		Any::Object v = a;
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
		Any::Array v = a;
		if(v.size()){};
	},Any::WrongTypeException);
	
	EXPECT_THROW({
		Any::Object v = a;
		if(v.size()){};
	},Any::WrongTypeException);
}

TEST(Any,ArrayConversion){
	Any::Array base{1,"foo",1.23};
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
		Any::Array v = a;
		EXPECT_EQ_ARRAY(base,v);
	});
	
	EXPECT_THROW({
		Any::Object v = a;
		if(v.size()){};
	},Any::WrongTypeException);
}

TEST(Any,ObjectConversion){
	Any::Object base{{"foo","bar"}};
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
		Any::Array v = a;
		if(v.size()){}
	},Any::WrongTypeException);
	
	EXPECT_NO_THROW({
		Any::Object v = a;
		EXPECT_EQ_OBJECT(base,v);
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
		Any a = Any::Array{};
	});
	EXPECT_NO_THROW({
		Any a = Any::Object{};
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
		a = Any::Array{};
	},Any::WrongTypeException);

	EXPECT_THROW({
		a = Any::Object{};
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
		a = Any::Array{};
	},Any::WrongTypeException);

	EXPECT_THROW({
		a = Any::Object{};
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
		a = Any::Array{};
	},Any::WrongTypeException);

	EXPECT_THROW({
		a = Any::Object{};
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
		a = Any::Array{};
	},Any::WrongTypeException);

	EXPECT_THROW({
		a = Any::Object{};
	},Any::WrongTypeException);
}

TEST(Any,ArrayWrongAssignment){
	Any a{Any::Array{}};
	
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
		a = Any::Array{};
	});

	EXPECT_THROW({
		a = Any::Object{};
	},Any::WrongTypeException);
}

TEST(Any,ObjectWrongAssignment){
	Any a{Any::Object{}};
	
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
		a = Any::Array{};
	},Any::WrongTypeException);

	EXPECT_NO_THROW({
		a = Any::Object{};
	});
}

TEST(Any,ObjectIndexOperators){
	Any a{Any::Object{}};
	EXPECT_NO_THROW({
		a["foo"] = "bar";
	});
	EXPECT_EQ("bar",static_cast<std::string>(a["foo"]));
	EXPECT_THROW({
		a[0] = 123;
	},Any::WrongTypeException);
}

TEST(Any,ArrayIndexOperators){
	Any a{Any::Array{}};
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

TEST(Any, toString){
	Any a;
	EXPECT_EQ("null",a.toString());
	Any b{true};
	EXPECT_EQ("true",b.toString());
	Any c{false};
	EXPECT_EQ("false",c.toString());
	Any d{123};
	EXPECT_EQ("123",d.toString());
	Any e{123.45};
	EXPECT_EQ("123.450000",e.toString());
	Any f{Any::Array{}};
	EXPECT_EQ("[]",f.toString());
	Any g{Any::Object{}};
	EXPECT_EQ("{}",g.toString());

	Any h{Any::Array{1,2,3}};
	EXPECT_EQ("[1,2,3]",h.toString());

	Any i{Any::Object{{"foo","bar"}}};
	EXPECT_EQ("{\"foo\":\"bar\"}",i.toString());

	Any j{Any::Object{
		{"array",Any::Array{Any::Object{{"foo","bar"}}}}
	}};
	EXPECT_EQ("{\"array\":[{\"foo\":\"bar\"}]}",j.toString());
}

TEST(Any, JsonEncoderDecoderTest){
	Any a = Any::Object{
		{"undefined",Any{}},
		{"true",Any{true}},
		{"false",Any{false}},
		{"integer",Any{23}},
		{"double",Any{12.34}},
		{"string",Any{"foobar"}},
		{"array",Any::Array{1,2,Any::Object{{"foo","bar"}},Any::Array{1,2,3}}},
		{"object",Any::Object{{"foo","bar"}}}
	};
	std::string jsonEncoded = a.toString();
	Any b = Any::fromString(jsonEncoded);
	EXPECT_EQ(a,b);
}

TEST(Any,ArrayHelper){
	Any a{Any::Array{}};
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

TEST(Any,BenchmarkUndefinedOneMillion){
	for(int i=0;i<1000000;i++){
		Any a;
	}
}

TEST(Any,BenchmarkBoolOneMillion){
	for(int i=0;i<1000000;i++){
		Any a{true};
		bool b = a;
		if(b);
	}
}

TEST(Any,BenchmarkIntegerOneMillion){
	for(int i=0;i<1000000;i++){
		Any a{23};
		int b = a;
		if(b);
	}
}

TEST(Any,BenchmarkDoubleOneMillion){
	for(int i=0;i<1000000;i++){
		Any a{23.45};
		double b = a;
		if(b);
	}
}

TEST(Any,BenchmarkStringOneMillion){
	for(int i=0;i<1000000;i++){
		Any a{""};
		std::string b = a;
		if(b.size());
	}
}

TEST(Any,BenchmarkArrayOneMillion){
	for(int i=0;i<1000000;i++){
		Any a{Any::Array{}};
		Any::Array b = a;
		if(b.size());
	}
}

TEST(Any,BenchmarkObjectOneMillion){
	for(int i=0;i<1000000;i++){
		Any a{Any::Object{}};
		Any::Object b = a;
		if(b.size());
	}
}
