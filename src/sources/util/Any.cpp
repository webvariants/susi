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
	type = BOOL;
	std::swap(this->boolValue,value);
}
void Any::operator=(int && value) {
	type = INTEGER;
	std::swap(this->integerValue,value);
}
void Any::operator=(double && value) {
	type = DOUBLE;
	std::swap(this->doubleValue,value);
}

void Any::operator=(std::string && value) {
	type = STRING;
	std::swap(this->stringValue,value);
}
void Any::operator=(std::deque<Any> && value) {
	type = ARRAY;
	std::swap(this->arrayValue,value);
}
void Any::operator=(std::map<std::string,Any> && value) {
	type = OBJECT;
	std::swap(this->objectValue,value);
}

// copy asignment operators;
void Any::operator=(const bool & value) {
	if(this->type != BOOL){
		throw AnyWrongTypeException(BOOL, this->type);
	}	
	this->type = BOOL;
	this->boolValue = value;
}

void Any::operator=(const int & value) {
	if(this->type != INTEGER){
		throw AnyWrongTypeException(INTEGER, this->type);
	}
	this->type = INTEGER;
	this->integerValue = value;
}

void Any::operator=(const double & value) {
	if(this->type != DOUBLE){
		throw AnyWrongTypeException(DOUBLE, this->type);
	}
	this->type = DOUBLE;
	this->doubleValue = value;
}

void Any::operator=(const std::string & value) {
	if(this->type != STRING){
		throw AnyWrongTypeException(STRING, this->type);
	}
	this->type = STRING;
	this->stringValue = value;
}

void Any::operator=(const std::deque<Any> & value) {
	if(this->type != ARRAY){
		throw AnyWrongTypeException(ARRAY, this->type);
	}
	this->type = ARRAY;
	this->arrayValue = value;

}
void Any::operator=(const std::map<std::string,Any> & value) {
	if(this->type != OBJECT){
		throw AnyWrongTypeException(OBJECT, this->type);
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
unsigned char Any::getType(){
	return this->type;
}

// index operators 
Any& Any::operator[](const int pos) {
	std::cout<<"index operators"<<" "<<ARRAY<<std::endl;
	if(type != ARRAY) {
		throw AnyWrongTypeException(ARRAY, type);
	} 

	return this->arrayValue[pos];
}

Any& Any::operator[](std::string key) {
	if(type != OBJECT) {
		throw AnyWrongTypeException(OBJECT, type);
	} 

	return this->objectValue[key];
}

//reference conversion operators
Any::operator bool&() {
	if(this->type != BOOL) {
		throw AnyWrongTypeException(BOOL, type);
	}

	return this->boolValue;
}
Any::operator int&() {
	if(this->type != INTEGER) {
		throw AnyWrongTypeException(INTEGER, type);
	}

	return this->integerValue;
}
Any::operator double&() {
	if(this->type != DOUBLE) {
		throw AnyWrongTypeException(DOUBLE, type);
	}

	return this->doubleValue;
}
Any::operator std::string&() {
	if(this->type != STRING) {
		throw AnyWrongTypeException(STRING, type);
	}

	return this->stringValue;
}
Any::operator std::deque<Any>&() {
	if(this->type != ARRAY) {
		throw AnyWrongTypeException(ARRAY, type);
	}

	return this->arrayValue;
}
Any::operator std::map<std::string,Any>&() {
	if(this->type != OBJECT) {
		throw AnyWrongTypeException(OBJECT, type);
	}

	return this->objectValue;
}