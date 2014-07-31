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
#include <util/Any.h>

using namespace Susi::Util;

Any::Any() {
	this->type = UNDEFINED;
}

// copy ctor's
Any::Any(const bool & value) {
	this->type = BOOL;
	this->boolValue = value;
}

Any::Any(const int & value) {
	this->type = INTEGER;
	this->integerValue = value;
}

Any::Any(const double & value) {
	this->type = DOUBLE;
	this->doubleValue = value;
}

Any::Any(const std::string & value) {
	this->type = STRING;
	this->stringValue = value;
}

Any::Any(const std::deque<Any> & value) {
	this->type = ARRAY;
	this->arrayValue = value;

}
Any::Any(const std::map<std::string,Any> & value) {
	this->type = OBJECT;
	this->objectValue = value;
}

Any::Any(const Any & value) {
	this->type = value.type;

	switch(this->type) {
		case UNDEFINED:
			break;
		case BOOL:
			this->boolValue = value.boolValue;
			break;
		case INTEGER:
			this->integerValue = value.integerValue;
			break;
		case DOUBLE:
			this->doubleValue = value.doubleValue;
			break;
		case STRING:
			this->stringValue = value.stringValue;
			break;
		case ARRAY:
			this->arrayValue = value.arrayValue;
			break;
		case OBJECT:
			this->objectValue = value.objectValue;
			break;
	}
}

// move ctor's
Any::Any(Any && value) {
	*this = value;
	value.type = UNDEFINED;
}

Any::Any(bool && value) {
	type = BOOL;
	std::swap(this->boolValue,value);
}
Any::Any(int && value) {
	type = INTEGER;
	std::swap(this->integerValue,value);
}
Any::Any(double && value) {
	type = DOUBLE;
	std::swap(this->doubleValue,value);
}

Any::Any(std::string && value) {
	type = STRING;
	std::swap(this->stringValue,value);
}
Any::Any(std::deque<Any> && value) {
	type = ARRAY;
	std::swap(this->arrayValue,value);
}
Any::Any(std::map<std::string,Any> && value) {
	type = OBJECT;
	std::swap(this->objectValue,value);
}

// move asignment operator
void Any::operator=(Any && value) {
	std::swap(value.type , type);

	switch(this->type) {
		case UNDEFINED:
			break;
		case BOOL:
			std::swap(value.boolValue , boolValue);
			break;
		case INTEGER:
			std::swap(value.integerValue , integerValue);
			break;
		case DOUBLE:
			std::swap(value.doubleValue , doubleValue);
			break;
		case STRING:
			std::swap(value.stringValue , stringValue);
			break;
		case ARRAY:			
			std::swap(value.arrayValue , arrayValue);
			break;
		case OBJECT:
			std::swap(value.objectValue , objectValue);
			break;
	}

	value.type = UNDEFINED;
}

void Any::operator=(bool && value) {
	if(this->type != UNDEFINED && this->type != BOOL){
		throw WrongTypeException(BOOL, this->type);
	}
	type = BOOL;
	std::swap(this->boolValue,value);
}
void Any::operator=(int && value) {
	if(this->type != UNDEFINED && this->type != INTEGER){
		throw WrongTypeException(INTEGER, this->type);
	}
	type = INTEGER;
	std::swap(this->integerValue,value);
}
void Any::operator=(double && value) {
	if(this->type != UNDEFINED && this->type != DOUBLE){
		throw WrongTypeException(DOUBLE, this->type);
	}
	type = DOUBLE;
	std::swap(this->doubleValue,value);
}

void Any::operator=(std::string && value) {
	if(this->type != UNDEFINED && this->type != STRING){
		throw WrongTypeException(STRING, this->type);
	}
	type = STRING;
	std::swap(this->stringValue,value);
}
void Any::operator=(std::deque<Any> && value) {
	if(this->type != UNDEFINED && this->type != ARRAY){
		throw WrongTypeException(ARRAY, this->type);
	}
	type = ARRAY;
	std::swap(this->arrayValue,value);
}
void Any::operator=(std::map<std::string,Any> && value) {
	if(this->type != UNDEFINED && this->type != OBJECT){
		throw WrongTypeException(OBJECT, this->type);
	}
	type = OBJECT;
	std::swap(this->objectValue,value);
}

// copy asignment operators;
void Any::operator=(const bool & value) {
	if(this->type != UNDEFINED && this->type != BOOL){
		throw WrongTypeException(BOOL, this->type);
	}	
	this->type = BOOL;
	this->boolValue = value;
}

void Any::operator=(const int & value) {
	if(this->type != UNDEFINED && this->type != INTEGER){
		throw WrongTypeException(INTEGER, this->type);
	}
	this->type = INTEGER;
	this->integerValue = value;
}

void Any::operator=(const double & value) {
	if(this->type != UNDEFINED && this->type != DOUBLE){
		throw WrongTypeException(DOUBLE, this->type);
	}
	this->type = DOUBLE;
	this->doubleValue = value;
}

void Any::operator=(const std::string & value) {
	if(this->type != UNDEFINED && this->type != STRING){
		throw WrongTypeException(STRING, this->type);
	}
	this->type = STRING;
	this->stringValue = value;
}

void Any::operator=(const char * value) {
	std::string v{value};
	*this = v;
}

void Any::operator=(const std::deque<Any> & value) {
	if(this->type != UNDEFINED && this->type != ARRAY){
		throw WrongTypeException(ARRAY, this->type);
	}
	this->type = ARRAY;
	this->arrayValue = value;

}
void Any::operator=(const std::map<std::string,Any> & value) {
	if(this->type != UNDEFINED && this->type != OBJECT){
		throw WrongTypeException(OBJECT, this->type);
	}
	this->type = OBJECT;
	this->objectValue = value;
}

void Any::operator=(const Any & value) {
	this->type = value.type;

	switch(this->type) {
		case UNDEFINED:
			break;
		case BOOL:
			this->boolValue = value.boolValue;
			break;
		case INTEGER:
			this->integerValue = value.integerValue;
			break;
		case DOUBLE:
			this->doubleValue = value.doubleValue;
			break;
		case STRING:
			this->stringValue = value.stringValue;
			break;
		case ARRAY:
			this->arrayValue = value.arrayValue;
			break;
		case OBJECT:
			this->objectValue = value.objectValue;
			break;
	}
}

// type test operators
bool Any::isNull() {
	return type == UNDEFINED;
}
bool Any::isBool() {
	return type == BOOL;
}
bool Any::isInteger(){
	return type == INTEGER;
}
bool Any::isDouble(){
	return type == DOUBLE;
}
bool Any::isString(){
	return type == STRING;
}
bool Any::isArray(){
	return type == ARRAY;
}
bool Any::isObject(){
	return type == OBJECT;
}
int Any::getType(){
	return this->type;
}

// deque operators
void Any::push_back(Any & value) {
	if(type != ARRAY) {
		throw WrongTypeException(ARRAY, type);
	}

	this->arrayValue.push_back(value);
}

void Any::push_front(Any & value) {
	if(type != ARRAY) {
		throw WrongTypeException(ARRAY, type);
	}

	this->arrayValue.push_front(value);
}


void Any::push_back(Any && value){
	push_back(value);
}
void Any::push_front(Any && value){
	push_front(value);
}

void Any::pop_back() {
	if(type != ARRAY) {
		throw WrongTypeException(ARRAY, type);
	}

	this->arrayValue.pop_back();
}

void Any::pop_front() {
	if(type != ARRAY) {
		throw WrongTypeException(ARRAY, type);
	}

	this->arrayValue.pop_front();
}

size_t Any::size() const {
	if(type != ARRAY && type != OBJECT) {

		throw WrongTypeException(ARRAY, type);
	} 

	if(type == ARRAY) {
		return this->arrayValue.size();
	}

	if(type == OBJECT) {
		return this->objectValue.size();
	}

	return 0;
}

// index operators 
Any& Any::operator[](const int pos) {
	if(type != ARRAY) {
		throw WrongTypeException(ARRAY, type);
	} 
	return this->arrayValue[pos];
}

Any& Any::operator[](std::string key){
	if(type != OBJECT) {
		throw WrongTypeException(OBJECT, type);
	}
	return this->objectValue[key];
}

Any& Any::operator[](const char * key){
	std::string k{key};
	return ((*this)[k]);
}

//reference conversion operators
Any::operator bool&() {
	if(this->type != BOOL) {
		throw WrongTypeException(BOOL, type);
	}

	return this->boolValue;
}
Any::operator int&() {
	if(this->type != INTEGER) {
		throw WrongTypeException(INTEGER, type);
	}

	return this->integerValue;
}
Any::operator double&() {
	if(this->type != DOUBLE) {
		throw WrongTypeException(DOUBLE, type);
	}

	return this->doubleValue;
}
Any::operator std::string&() {
	if(this->type != STRING) {
		throw WrongTypeException(STRING, type);
	}

	return this->stringValue;
}
Any::operator std::deque<Any>&() {
	if(this->type != ARRAY) {
		throw WrongTypeException(ARRAY, type);
	}

	return this->arrayValue;
}
Any::operator std::map<std::string,Any>&() {
	if(this->type != OBJECT) {
		throw WrongTypeException(OBJECT, type);
	}

	return this->objectValue;
}


bool Any::operator==(const Any & other) const {
	if(type != other.type)return false;
	switch(type){
		case UNDEFINED: return true;
		case BOOL: return boolValue==other.boolValue;
		case INTEGER: return integerValue==other.integerValue;
		case DOUBLE: return doubleValue==other.doubleValue;
		case STRING: return stringValue==other.stringValue;
		case ARRAY: {
			if(size()!=other.size())return false;
			for(size_t i=0;i<size();i++){
				if(arrayValue[i]!=other.arrayValue.at(i))return false;
			}
			return true;
		}
		case OBJECT:{
			for(auto & kv : objectValue){
				if(kv.second != other.objectValue.at(kv.first)){
					return false;
				}
			}
			return true;
		}
		default: return false;
	}
}
bool Any::operator!=(const Any & other) const{
	return !(*this == other);
}
// json de/encoder; 
std::string Any::toString() {
	std::string result = "";
	result = this->__parseToString(result);

	return result;
}

std::string Any::__parseToString(std::string & current) {
	int max_size;
	std::string result = "";

	switch(this->type) {
		case UNDEFINED:
			result = "null";
			break;
		case BOOL:
			result = boolValue ? "true" : "false";
			break;
		case INTEGER:
			result = std::to_string(integerValue);
			break;			
		case DOUBLE:
			result = std::to_string(doubleValue);
			break;
		case STRING:
			result += "\"";
			result += stringValue;
			result += "\"";
			break;
		case ARRAY:
			max_size = this->size() - 1;
			result = "[";
			for (int i = 0; i <= max_size; ++i) {
				result = arrayValue[i].__parseToString(result);

				if(i < max_size) {
					result += ",";
				}
			}
			result += "]";
			break;
		case OBJECT:
			result += "{";
			for (std::map<std::string,Any>::iterator it=objectValue.begin(); it!=objectValue.end(); ++it) {
				result += "\"";
				result += it->first;
				result += "\":";
				result = objectValue[it->first].__parseToString(result);
			}
			result += "}";
			break;
	}

	return current + "" + result;
}

static Any fromString(std::string str) {
	Any result = Any();
	
	const char *js;
	int r;
	jsmn_parser p;
	jsmntok_t tokens[10];

	js = "{\"a\": 0}";

	jsmn_init(&p);
	r = jsmn_parse(&p, js, strlen(js), tokens, 10);

	for(int i = 0; i < 10; ++i) {
		std::cout<<token[i]<<std::endl;
	}
	
	/*
	check(r >= 0);
	check(TOKEN_EQ(tokens[0], 0, 8, JSMN_OBJECT));
	check(TOKEN_EQ(tokens[1], 2, 3, JSMN_STRING));
	check(TOKEN_EQ(tokens[2], 6, 7, JSMN_PRIMITIVE));

	check(TOKEN_STRING(js, tokens[0], js));
	check(TOKEN_STRING(js, tokens[1], "a"));
	check(TOKEN_STRING(js, tokens[2], "0"));
    */
	jsmn_init(&p);
	js = "[\"a\":{},\"b\":{}]";
	r = jsmn_parse(&p, js, strlen(js), tokens, 10);
	//check(r >= 0);

	jsmn_init(&p);
	js = "{\n \"Day\": 26,\n \"Month\": 9,\n \"Year\": 12\n }";
	r = jsmn_parse(&p, js, strlen(js), tokens, 10);
	//check(r >= 0);

	return result;
}