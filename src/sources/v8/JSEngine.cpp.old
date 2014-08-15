#include "v8/JSEngine.h"

using namespace v8;

Handle<Value> Susi::convertFromCPP(Poco::Dynamic::Var cppVal){
	if(cppVal.isEmpty()){
		return Handle<Value>{};
	}
	auto isolate = Isolate::GetCurrent();
	Handle<Context> context = isolate->GetCurrentContext();
    Handle<Object> global = context->Global();
 	Handle<Object> JSON = global->Get(String::NewFromUtf8(isolate,"JSON"))->ToObject();
    Handle<Function> JSON_parse = Handle<Function>::Cast(JSON->Get(String::NewFromUtf8(isolate,"parse")));
	Handle<Value> arguments[1];
	arguments[0] = String::NewFromUtf8(isolate,cppVal.toString().c_str()); 
	return JSON_parse->Call(JSON, 1, arguments);
}
Handle<Value> Susi::convertFromCPP(std::string cppVal){
	auto isolate = Isolate::GetCurrent();
	Handle<Context> context = isolate->GetCurrentContext();
    Handle<Object> global = context->Global();
 	Handle<Object> JSON = global->Get(String::NewFromUtf8(isolate,"JSON"))->ToObject();
    Handle<Function> JSON_parse = Handle<Function>::Cast(JSON->Get(String::NewFromUtf8(isolate,"parse")));
	Handle<Value> arguments[1];
	arguments[0] = String::NewFromUtf8(isolate,cppVal.c_str()); 
	return JSON_parse->Call(JSON, 1, arguments);
}

Poco::Dynamic::Var Susi::convertFromJS(Handle<Value> jsVal){
	if(jsVal->IsArray()){
		std::vector<Poco::Dynamic::Var> result;
		auto obj = jsVal->ToObject();
		const Local<Array> props = obj->GetPropertyNames();
		const uint32_t length = props->Length();
		for (uint32_t i=0 ; i<length ; ++i){
			const Local<Value> key = props->Get(i);
			const Local<Value> value = obj->Get(key);
			result.push_back(Susi::convertFromJS(value));
		}
		return Poco::Dynamic::Var{result};
	}
	if(jsVal->IsObject()){
		Poco::Dynamic::Var result{Poco::Dynamic::Struct<std::string>{}};
		auto obj = jsVal->ToObject();
		const Local<Array> props = obj->GetPropertyNames();
		const uint32_t length = props->Length();
		for (uint32_t i=0 ; i<length ; ++i){
			const Local<Value> key = props->Get(i);
			const Local<Value> value = obj->Get(key);
			String::Utf8Value keyStr(key);
			result[std::string(*keyStr)] = Susi::convertFromJS(value);
		}
		return result;
	}
	if(jsVal->IsString()){
		String::Utf8Value val(jsVal);
		Poco::Dynamic::Var result{std::string(*val)};
		return result;
	}
	if(jsVal->IsNumber()){
		Poco::Dynamic::Var result{jsVal->ToNumber()->Value()};
		return result;
	}
	if(jsVal->IsBoolean()){
		Poco::Dynamic::Var result{jsVal->ToBoolean()->Value()};
		return result;
	}
	if(jsVal->IsNativeError()){
		String::Utf8Value val(jsVal);
		Poco::Dynamic::Var result{std::string(*val)};
		return result;	
	}
	if(jsVal->IsUndefined()){
		Poco::Dynamic::Var result;
		return result;
	}
	return Poco::Dynamic::Var{"type not known"};	
}

Handle<Value> Susi::JSEngine::run(std::string code){
	Handle<String> source = String::NewFromUtf8(isolate, code.c_str());
  	Handle<Script> script = Script::Compile(source);
  	Handle<Value> result = script->Run();	
  	return result;
}


Handle<Value> Susi::JSEngine::runFile(std::string filename){
	if(filename!=""){
  		std::ifstream t(filename);
		std::string str((std::istreambuf_iterator<char>(t)),
             			 std::istreambuf_iterator<char>());
		if(str!=""){
			return run(str);
		}else{
			std::cerr<<"no such js source file: "<<filename<<std::endl;
		}
  	}
  	return Undefined(isolate);
}


void Susi::JSEngine::Log(const v8::FunctionCallbackInfo<v8::Value>& args) {
	if (args.Length() < 1) return;
	HandleScope scope(args.GetIsolate());
	Handle<Value> arg = args[0];
	std::string str{Susi::convertFromJS(arg).toString()};
	std::cout<<str<<std::endl;
	args.GetReturnValue().Set(arg);
	return;
}

void Susi::JSEngine::Publish(const v8::FunctionCallbackInfo<v8::Value>& args) {
	std::cout<<"in publish!"<<std::endl;
	if (args.Length() < 1) return;
	HandleScope scope(args.GetIsolate());
	Handle<Value> topicValue = args[0];
	std::string topic{Susi::convertFromJS(topicValue).toString()};
	Susi::Event event(topic);
	if (args.Length() >= 2) {
		Handle<Value> payloadValue = args[1];
		auto payload = Susi::convertFromJS(payloadValue);
		event.payload = payload;
	}
	Susi::publish(event);
	args.GetReturnValue().Set(true);
	return;
}

void Susi::JSEngine::Subscribe(const v8::FunctionCallbackInfo<v8::Value>& args) {
	if (args.Length() < 2) return;
	Handle<Value> callbackValue = args[1];
	if(callbackValue->IsFunction()){
		Handle<Value> topicValue = args[0];
		std::string topic{Susi::convertFromJS(topicValue).toString()};
		std::shared_ptr<Persistent<Function>> jsCallback{
			new Persistent<Function>(Isolate::GetCurrent(),Handle<Function>::Cast(callbackValue))
		};
		auto callback = [jsCallback](Susi::Event & event){
			Local<Function> func = Local<Function>::New(Isolate::GetCurrent(),*jsCallback);
			Handle<Value> callbackArguments[1];
			callbackArguments[0] = Susi::convertFromCPP(event.toString());
			TryCatch trycatch;
			auto res = func->Call(func,1,callbackArguments);
			if (res.IsEmpty()) {  
				Handle<Value> exception = trycatch.Exception();
				String::Utf8Value exception_str(exception);
				std::cout<<"Exception: "<<*exception_str<<std::endl;
			}
		};
		long id = Susi::subscribe(topic,callback);
		args.GetReturnValue().Set((double)id);
	}else{
		args.GetReturnValue().Set(false);
	}
}

void Susi::JSEngine::Unsubscribe(const v8::FunctionCallbackInfo<v8::Value>& args){
	Handle<Value> topicValue = args[0];
	Handle<Value> idValue = args[1];
	if(topicValue->IsString() && idValue->IsNumber()){
		std::string topic{Susi::convertFromJS(topicValue).toString()};
		long id{(long)Susi::convertFromJS(idValue).extract<double>()};
		Susi::unsubscribe(topic,id);
	}		
}