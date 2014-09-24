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

Any::Any(const long & value) {
	this->type = INTEGER;
	this->integerValue = value;
}

Any::Any(const long long & value) {
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
	*this = std::move(value);
}

Any::Any(bool && value) {
	type = BOOL;
	std::swap(this->boolValue,value);
}
Any::Any(int && value) {
	type = INTEGER;
	this->integerValue = value;
	value = 0;
}

Any::Any(long && value) {
	type = INTEGER;
	this->integerValue = value;
	value = 0;
}

Any::Any(long long && value) {
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
	this->integerValue = value;
	value = 0;
}

void Any::operator=(long && value) {
	if(this->type != UNDEFINED && this->type != INTEGER){
		throw WrongTypeException(INTEGER, this->type);
	}
	type = INTEGER;
	this->integerValue = value;
	value = 0;
}

void Any::operator=(long long && value) {
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

void Any::operator=(const long & value) {
	if(this->type != UNDEFINED && this->type != INTEGER){
		throw WrongTypeException(INTEGER, this->type);
	}
	this->type = INTEGER;
	this->integerValue = value;
}

void Any::operator=(const long long & value) {
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
void Any::push_back(Any value) {
	if(type != ARRAY && type != UNDEFINED) {
		throw WrongTypeException(ARRAY, type);
	}
	if(type == UNDEFINED) {
		type = ARRAY;
	}
	this->arrayValue.push_back(value);
}

void Any::push_front(Any value) {
	if(type != ARRAY && type != UNDEFINED) {
		throw WrongTypeException(ARRAY, type);
	}
	if(type == UNDEFINED) {
		type = ARRAY;
	}
	this->arrayValue.push_front(value);
}

/*
void Any::push_back(Any value){
	push_back(value);
}
void Any::push_front(Any value){
	push_front(value);
}*/

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

void Any::set(std::string key, Any value) {
	if(type == UNDEFINED) {
		type = OBJECT;
		this->objectValue = std::map<std::string,Any>{};
	}
	if(type == OBJECT) {
		std::vector<std::string> elems;
		Susi::Util::Helpers::split(key, '.', elems);
		auto * current = this;

		for(size_t e=0; e<elems.size()-1; e++){
			current = &(*current)[elems[e]];
			if((*current).isNull()){
				*current = Susi::Util::Any::Object{};
			}
		}
		(*current)[elems.back()] = value;
	} else {
		throw WrongTypeException(OBJECT, type);
	}
}

Any Any::get(std::string key) {

	if(type == OBJECT) {
		std::vector<std::string> elems;

		if(key.length() == 0) {
			return this;
		} else {
			Susi::Util::Helpers::split(key, '.', elems);

			Any found = this->objectValue;
			Any next  = this->objectValue;
			for(size_t e=0; e<elems.size(); e++)
			{
				std::string elem = elems[e];
				found = next[elem];
				next = found;
				if(found.getType() == Susi::Util::Any::UNDEFINED) {
					throw std::runtime_error("key doesn't exist!"+key);
				} else if(e == (elems.size()-2) && found.getType() != Susi::Util::Any::OBJECT) {
					throw WrongTypeException(OBJECT, type);
				}

			}
			return found;
		}
	} else {
		throw WrongTypeException(OBJECT, type);
	}
}


// index operators
Any& Any::operator[](const int pos) {
	if(type == UNDEFINED){
		type = ARRAY;
	}
	if(type != ARRAY) {
		throw WrongTypeException(ARRAY, type);
	}
	return this->arrayValue[pos];
}

Any& Any::operator[](std::string key){
	if(type == UNDEFINED){
		type = OBJECT;
	}
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

Any::operator long long&() {
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


// copy conversion operators
Any::operator int() {
	if(this->type != INTEGER) {
		throw WrongTypeException(INTEGER, type);
	}

	return static_cast<int>(this->integerValue);
}
Any::operator long() {
	if(this->type != INTEGER) {
		throw WrongTypeException(INTEGER, type);
	}

	return static_cast<long>(this->integerValue);
}
Any::operator std::map<std::string,std::string>(){
	if(this->type != OBJECT) {
		throw WrongTypeException(OBJECT, type);
	}
	std::map<std::string,std::string> result;
	for(auto & kv : objectValue){
		if(!kv.second.isNull()) { // empty object
			if(!kv.second.isString()) {
				throw WrongTypeException(STRING, type);
			}
			result[kv.first] = kv.second.stringValue;
		} else {
			result[kv.first] = "";
		}
	}
	return result;
}



bool Any::operator==(const Any & other) const {
	if(type != other.type)return false;
	try{
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
	}catch(...){
		return false;
	}
}
bool Any::operator!=(const Any & other) const{
	return !(*this == other);
}

std::string Any::toJSONString(){
	std::string result;
	switch(this->type) {
		case UNDEFINED: {
			result = "null";
			break;
		}
		case BOOL: {
			result = boolValue ? "true" : "false";
			break;
		}
		case INTEGER: {
			result = std::to_string(integerValue);
			break;
		}
		case DOUBLE: {
			result = std::to_string(doubleValue);
			break;
		}
		case STRING: {
			result += "\"";
			result += escapeJSON(stringValue);
			result += "\"";
			break;
		}
		case ARRAY: {
			size_t max_size = this->size() - 1;
			result = "[";
			if(this->size())for (size_t i = 0; i <= max_size; ++i) {
				result += arrayValue[i].toJSONString();
				if(i < max_size) {
					result += ",";
				}
			}
			result += "]";
			break;
		}
		case OBJECT: {
			result += "{";
			size_t current = 0;
			size_t max_size = size();
			for (auto & kv : objectValue) {
				result += "\"";
				result += escapeJSON(kv.first);
				result += "\":";
				result += kv.second.toJSONString();
				if(current++ < max_size-1){
					result += ",";
				}
			}
			result += "}";
			break;
		}
	}
	return result;
}

Any Any::fromJSONString(std::string str) {
	try{
		if(Susi::Util::Any::isJsonPrimitive(str)) {
			Susi::Util::Any v;
			if(Susi::Util::Helpers::isInteger(str)) {
				v = Susi::Util::Any{std::stoi(str)};
			} else if(Susi::Util::Helpers::isDouble(str)) {
				v = Susi::Util::Any{std::stod(str)};
			} else if(str=="true") {
				v = Susi::Util::Any{true};
			} else if(str=="false") {
				v = Susi::Util::Any{false};
			} else {
				//value is String
				v = str;
			}

			return v;
		} else {

			int r;
			jsmn_parser p;
			jsmntok_t tokens[JSON_TOKENS];
			jsmn_init(&p);
			r = jsmn_parse(&p, str.c_str(), str.size(), tokens, JSON_TOKENS);

			if(r < 0) {
				throw std::runtime_error{"Any::fromJSONString parse error"};
			}

			jsmntok_t * start = &tokens[0];
		    return tokenToAny(start,str.c_str());
		}
	}catch(...){
		return Susi::Util::Any{};
	}
}

// json helper
bool Any::isJsonPrimitive(std::string str){
	if(str.find("{") != std::string::npos ||
	   str.find("[") != std::string::npos ||
	   str.find("\"") != std::string::npos){
		return false;
	}
	return true;
}


Any Any::tokenToAny(jsmntok_t * & t, const  char *js){
	Any result;
	switch(t->type){
		case JSMN_OBJECT: {
			int objectSize = t->size;
			t++;
			result = Any::Object{};
			for(int i=0; i<objectSize; i+=2){
				std::string key = tokenToAny(t,js);
				Any value = tokenToAny(t,js);
				result[key] = value;
			}
			break;
		}
		case JSMN_ARRAY: {
			int arraySize = t->size;
			t++;
			result = Any::Array{};
			for(int i=0; i<arraySize; i++){
				Any value = tokenToAny(t,js);
				result.push_back(value);
			}
			break;
		}
		case JSMN_STRING: {
			std::string value{js+(t->start),js+(t->end)};
			result = value;
			t++;
			break;
		}
		case JSMN_PRIMITIVE: {
			std::string str{js+(t->start),js+(t->end)};
			if(str.compare("null") == 0) {
				result = Any{};
			} else if(str.compare("true") == 0) {
				result = true;
			} else if(str.compare("false") == 0) {
				result = false;
			} else if(str.find('.') != std::string::npos) {
				result = std::stod(str);
			} else {
				result = std::stoll(str);
			}
			t++;
			break;
		}
	}
	return result;
}

std::string Any::escapeJSON(const std::string& input) {
    std::string output;
    output.reserve(input.length());

    for (std::string::size_type i = 0; i < input.length(); ++i)
    {
        switch (input[i]) {
            case '"':
                output += "\\\"";
                break;
            case '/':
                output += "\\/";
                break;
            case '\b':
                output += "\\b";
                break;
            case '\f':
                output += "\\f";
                break;
            case '\n':
                output += "\\n";
                break;
            case '\r':
                output += "\\r";
                break;
            case '\t':
                output += "\\t";
                break;
            case '\\':
                output += "\\\\";
                break;
            default:
                output += input[i];
                break;
        }

    }

    return output;
}

std::string Any::unescapeJSON(const std::string& input) {
    Any::State s = Any::UNESCAPED;
    std::string output;
    output.reserve(input.length());

    for (std::string::size_type i = 0; i < input.length(); ++i)
    {
        switch(s)
        {
            case Any::ESCAPED:
                {
                    switch(input[i])
                    {
                        case '"':
                            output += '\"';
                            break;
                        case '/':
                            output += '/';
                            break;
                        case 'b':
                            output += '\b';
                            break;
                        case 'f':
                            output += '\f';
                            break;
                        case 'n':
                            output += '\n';
                            break;
                        case 'r':
                            output += '\r';
                            break;
                        case 't':
                            output += '\t';
                            break;
                        case '\\':
                            output += '\\';
                            break;
                        default:
                            output += input[i];
                            break;
                    }

                    s = Any::UNESCAPED;
                    break;
                }
            case Any::UNESCAPED:
                {
                    switch(input[i])
                    {
                        case '\\':
                            s = Any::ESCAPED;
                            break;
                        default:
                            output += input[i];
                            break;
                    }
                }
        }
    }
    return output;
}
