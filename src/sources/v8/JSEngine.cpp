#include "v8/JSEngine.h"

using namespace v8;

Handle<Value> Susi::JSEngine::convertFromCPP(std::string cppVal){
	auto isolate = Isolate::GetCurrent();
	Handle<Context> context = isolate->GetCurrentContext();
    Handle<Object> global = context->Global();
 	Handle<Object> JSON = global->Get(String::NewFromUtf8(isolate,"JSON"))->ToObject();
    Handle<Function> JSON_parse = Handle<Function>::Cast(JSON->Get(String::NewFromUtf8(isolate,"parse")));
	Handle<Value> arguments[1];
	arguments[0] = String::NewFromUtf8(isolate,cppVal.c_str());
	return JSON_parse->Call(JSON, 1, arguments);
}

Handle<Value> Susi::JSEngine::convertFromCPP(Susi::Util::Any cppVal){
	if(cppVal.isNull()){
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

Susi::Util::Any Susi::JSEngine::convertFromJS(Handle<Value> jsVal){
	if(jsVal->IsArray()){
		Susi::Util::Any::Array result;
		auto obj = jsVal->ToObject();
		const Local<Array> props = obj->GetPropertyNames();
		const uint32_t length = props->Length();
		for (uint32_t i=0 ; i<length ; ++i){
			const Local<Value> key = props->Get(i);
			const Local<Value> value = obj->Get(key);
			result.push_back(Susi::JSEngine::convertFromJS(value));
		}
		return result;
	}
	if(jsVal->IsObject()){
		Susi::Util::Any result = Susi::Util::Any::Object{};
		auto obj = jsVal->ToObject();
		const Local<Array> props = obj->GetPropertyNames();
		const uint32_t length = props->Length();
		for (uint32_t i=0 ; i<length ; ++i){
			const Local<Value> key = props->Get(i);
			const Local<Value> value = obj->Get(key);
			String::Utf8Value keyStr(key);
			result[std::string(*keyStr)] = Susi::JSEngine::convertFromJS(value);
		}
		return result;
	}
	if(jsVal->IsString()){
		String::Utf8Value val(jsVal);
		Susi::Util::Any result{std::string(*val)};
		return result;
	}
	if(jsVal->IsNumber()){
		Susi::Util::Any result{jsVal->ToNumber()->Value()};
		return result;
	}
	if(jsVal->IsBoolean()){
		Susi::Util::Any result{jsVal->ToBoolean()->Value()};
		return result;
	}
	if(jsVal->IsNativeError()){
		String::Utf8Value val(jsVal);
		Susi::Util::Any result{std::string(*val)};
		return result;
	}
	if(jsVal->IsUndefined()){
		Susi::Util::Any result;
		return result;
	}
	return Susi::Util::Any{"type not known"};
}


Local<Value> Susi::JSEngine::run(std::string code){
	Isolate* _isolate = Isolate::GetCurrent();
	EscapableHandleScope scope(_isolate);
	Local<String> _source = String::NewFromUtf8(_isolate, code.c_str());
  	Local<Script> _script = Script::Compile(_source);
  	Local<Value> _result = _script->Run();
  	return scope.Escape(_result);
}


Local<Value> Susi::JSEngine::runFile(std::string filename){
	if(filename!=""){
  		std::ifstream t(filename);
		std::string str((std::istreambuf_iterator<char>(t)),
             			 std::istreambuf_iterator<char>());
		if(str!=""){
			return run(str);
		}else{
			error("no such js source file: " + filename);
		}
  	}
  	return Undefined(isolate);
}


void Susi::JSEngine::Log(const v8::FunctionCallbackInfo<v8::Value>& args) {
	if (args.Length() < 1) return;
	HandleScope scope(args.GetIsolate());
	Handle<Value> arg = args[0];
	auto convertRes = Susi::JSEngine::convertFromJS(arg);
	std::string str{convertRes.toString()};
	log(str);
	args.GetReturnValue().Set(arg);
	return;
}

void Susi::JSEngine::Publish(const v8::FunctionCallbackInfo<v8::Value>& args) {
	log("JSEngine: in publish!");
	if (args.Length() < 1) return;
	HandleScope scope(args.GetIsolate());
	Handle<Value> topicValue = args[0];
	std::string topic{Susi::JSEngine::convertFromJS(topicValue).toString()};

	auto event = api_client->createEvent(topic);
	if (args.Length() >= 2) {
		Handle<Value> payloadValue = args[1];
		auto payload = Susi::JSEngine::convertFromJS(payloadValue);
		event->payload = payload;
	}
	api_client->publish(std::move(event));
	args.GetReturnValue().Set(true);
	return;
}

void Susi::JSEngine::RegisterConsumer(const v8::FunctionCallbackInfo<v8::Value>& args) {
	if (args.Length() < 2) return;
	Handle<Value> callbackValue = args[1];
	if(callbackValue->IsFunction()){
		Handle<Value> topicValue = args[0];
		std::string topic{Susi::JSEngine::convertFromJS(topicValue).toString()};
		std::shared_ptr<Persistent<Function>> jsCallback{new Persistent<Function>(Isolate::GetCurrent(),Handle<Function>::Cast(callbackValue))};
		Susi::Events::Consumer callback = [jsCallback](Susi::Events::SharedEventPtr event){
			Local<Function> func = Local<Function>::New(Isolate::GetCurrent(),*jsCallback);
			Handle<Value> callbackArguments[1];
			callbackArguments[0] = Susi::JSEngine::convertFromCPP(event->toAny());
			TryCatch trycatch;
			auto res = func->Call(func,1,callbackArguments);
			if (res.IsEmpty()) {
				Handle<Value> exception = trycatch.Exception();
				String::Utf8Value exception_str(exception);
				std::cout<<*exception_str<<std::endl;
			}
		};
		long id = api_client->subscribe(topic,callback);
		args.GetReturnValue().Set((double)id);
	}else{
		args.GetReturnValue().Set(false);
	}
}

void Susi::JSEngine::RegisterProcessor(const v8::FunctionCallbackInfo<v8::Value>& args) {
	if (args.Length() < 2) return;
	Handle<Value> callbackValue = args[1];
	if(callbackValue->IsFunction()){
		Handle<Value> topicValue = args[0];
		std::string topic{Susi::JSEngine::convertFromJS(topicValue).toString()};
		std::shared_ptr<Persistent<Function>> jsCallback{new Persistent<Function>(Isolate::GetCurrent(),Handle<Function>::Cast(callbackValue))};
		Susi::Events::Processor callback = [jsCallback](Susi::Events::SharedEventPtr event){
			Local<Function> func = Local<Function>::New(Isolate::GetCurrent(),*jsCallback);
			Handle<Value> callbackArguments[1];
			callbackArguments[0] = Susi::JSEngine::convertFromCPP(event->toAny());
			TryCatch trycatch;
			auto res = func->Call(func,1,callbackArguments);
			if (res.IsEmpty()) {
				Handle<Value> exception = trycatch.Exception();
				String::Utf8Value exception_str(exception);
				std::cout<<*exception_str<<std::endl;
			}
		};
		long id = api_client->subscribe(topic,callback);
		args.GetReturnValue().Set((double)id);
	}else{
		args.GetReturnValue().Set(false);
	}
}

// void Susi::JSEngine::Unsubscribe(const v8::FunctionCallbackInfo<v8::Value>& args){
// 	Handle<Value> topicValue = args[0];
// 	Handle<Value> idValue = args[1];
// 	if(topicValue->IsString() && idValue->IsNumber()){
// 		std::string topic{Susi::convertFromJS(topicValue).toString()};
// 		long id{(long)Susi::convertFromJS(idValue).extract<double>()};
// 		Susi::unsubscribe(topic,id);
// 	}
// }
