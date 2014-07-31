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

#ifndef __SUSI_ANY__
#define __SUSI_ANY__

#define BUFFER_SIZE 32768
#define JSON_TOKENS 256

#include <stdexcept>
#include <memory>
#include <deque>
#include <map>
#include <iostream>
#include <sstream>
#include <string>
 

#include <stdio.h>
#include <stdlib.h>
#include <cstring>

#include "util/jsmn.h"

namespace Susi {
	namespace Util {
		class Any {	
		public:
		enum Type {
			 UNDEFINED,
			 BOOL,
			 INTEGER,
			 DOUBLE,
			 STRING,
			 ARRAY,
			 OBJECT
		};
		protected:	
			Type type;

			bool boolValue;
			int integerValue;
			double doubleValue;
			std::string stringValue;
			std::deque<Any> arrayValue;
			std::map<std::string,Any> objectValue;

		public:		
			class WrongTypeException: public std::exception {
				protected:
					Type _expected;
					Type _actual;
				public:
					WrongTypeException(Type expected, Type actual) 
						: _expected{expected},
						  _actual{actual} {}
					virtual const char* what() noexcept {
						std::string msg = "wrong type. expected: ";
						msg += typeToString(_expected);
						msg += " actual: ";
						msg += typeToString(_actual);
						return msg.c_str();
					}
					std::string typeToString(Type type){
						switch(type){
							case UNDEFINED: return "UNDEFINED";
							case BOOL: return "BOOL";
							case INTEGER: return "INTEGER";
							case DOUBLE: return "DOUBLE";
							case STRING: return "STRING";
							case ARRAY: return "ARRAY";
							case OBJECT: return "OBJECT";
						}
						return "";
					}
			};

			Any(); // undefined ctor;
			
			// copy ctor's
			Any(const Any & value);
			Any(const bool & value);
			Any(const int & value);
			Any(const double & value);
			Any(const std::string & value);
			Any(const char* value) : Any(std::string{value}) {}
			Any(const std::deque<Any> & value);
			Any(const std::map<std::string,Any> & value);

			// move ctor's
			Any(Any && value);
			Any(bool && value);
			Any(int && value);
			Any(double && value);
			Any(std::string && value);
			Any(std::deque<Any> && value);
			Any(std::map<std::string,Any> && value);
			
			// move asignment operator
			void operator=(Any && value);
			void operator=(bool && value);
			void operator=(int && value);
			void operator=(double && value);
			void operator=(std::string && value);
			void operator=(std::deque<Any> && value);
			void operator=(std::map<std::string,Any> && value);

			// copy asignment operators;
			void operator=(const Any & value);
			void operator=(const bool & value);
			void operator=(const int & value);
			void operator=(const double & value);
			void operator=(const std::string & value);
			void operator=(const char* value);
			void operator=(const std::deque<Any> & value);
			void operator=(const std::map<std::string,Any> & value);

			// index operators
			Any& operator[](const int pos);
			Any& operator[](std::string key);
			Any& operator[](const char * key);				


			//reference conversion operators
			operator bool&(); // Any foo(true); bool & value = foo; value = false; EXPECT_FALSE(foo);
			operator int&();
			operator double&();
			operator std::string&();
			operator std::deque<Any>&();
			operator std::map<std::string,Any>&();
			

			bool operator==(const Any & other) const;
			bool operator!=(const Any & other) const;

			// type test operators
			int  getType();
			bool isNull();
			bool isBool();
			bool isInteger();
			bool isDouble();
			bool isString();
			bool isArray();
			bool isObject();

			// deque operators
			void push_back(Any & value);
			void push_front(Any & value);
			void push_back(Any && value);
			void push_front(Any && value);
			void pop_back();
			void pop_front();
			size_t size() const;

			// json de/encoder; 
			std::string toString();
			std::string __parseToString(std::string & current);
			static Any fromString(std::string str);
			static Any __parseFromString(Any & current, int ii, int nz, const char *js, jsmntok_t *tokens);

		protected:
			// json helper
			static std::string json_token_tostr(const char *js, jsmntok_t *t);
		};
	}
}

#endif // __SUSI_ANY__