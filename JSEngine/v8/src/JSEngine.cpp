#include "JSEngine.h"

using namespace v8;

Susi::JS::Engine *Susi::JS::engine{nullptr};

Handle<Value> Susi::JS::Engine::convertFromCPP(Susi::Util::Any cppVal){
	HandleScope scope;
	if(cppVal.isNull()){
		return Handle<Value>{};
	}
	std::string stdStr = cppVal.toString();
	Handle<Value> str = String::New(stdStr.c_str());
    Handle<Context> context = Susi::JS::engine->context;
    Handle<Object> global = context->Global();
    Handle<Object> JSON = global->Get(String::New("JSON"))->ToObject();
    Handle<Function> JSON_parse = Handle<Function>::Cast(JSON->Get(String::New("parse")));
    return scope.Close(JSON_parse->Call(JSON, 1, &str));
}

Susi::Util::Any Susi::JS::Engine::convertFromJS(Handle<Value> jsVal){
	Locker locker{Susi::JS::engine->isolate};
	
	if(jsVal->IsArray()){
		Susi::Util::Any::Array result;
		auto obj = jsVal->ToObject();
		const Local<Array> props = obj->GetPropertyNames();
		const uint32_t length = props->Length();
		for (uint32_t i=0 ; i<length ; ++i){
			const Local<Value> key = props->Get(i);
			const Local<Value> value = obj->Get(key);
			result.push_back(Susi::JS::Engine::convertFromJS(value));
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
			result[std::string(*keyStr)] = Susi::JS::Engine::convertFromJS(value);
		}
		return result;
	}
	if(jsVal->IsString()){
		String::Utf8Value val{jsVal};
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


Handle<Value> Susi::JS::Engine::run(std::string code){
	v8::Context::Scope context_scope(context);
	HandleScope scope;
	Local<String> _source = String::New(code.c_str());
  	Local<Script> _script = Script::Compile(_source);
  	Handle<Value> _result = _script->Run();
  	return scope.Close(_result);
}


Handle<Value> Susi::JS::Engine::runFile(std::string filename){
	if(filename!=""){
  		std::ifstream t(filename);
		std::string str((std::istreambuf_iterator<char>(t)),
             			 std::istreambuf_iterator<char>());
		if(str!=""){
			return run(str);
		}else{
			Susi::Logger::error("no such js source file: " + filename);
		}
  	}
  	return Undefined();
}


Handle<Value> Susi::JS::Engine::Log(const v8::Arguments& args) {
	Locker locker{Susi::JS::engine->isolate};
	if (args.Length() < 1) return Undefined();
	HandleScope scope;
	Handle<Value> arg = args[0];
	auto convertRes = Susi::JS::Engine::convertFromJS(arg);
	std::string str{convertRes.toString()};
	Susi::Logger::log(str);
	return arg;
}

Handle<Value> Susi::JS::Engine::Publish(const v8::Arguments& args) {
	Locker locker{Susi::JS::engine->isolate};
	
	Susi::Logger::log("JS::Engine: in publish!");
	if (args.Length() < 1) return False();
	HandleScope scope;
	Handle<Value> topicValue = args[0];
	std::string topic{Susi::JS::Engine::convertFromJS(topicValue).toString()};

	auto event = Susi::JS::engine->susi_client.createEvent(topic);
	if (args.Length() >= 2) {
		Handle<Value> payloadValue = args[1];
		auto payload = Susi::JS::Engine::convertFromJS(payloadValue);
		event->payload = payload;
	}
	Susi::JS::engine->susi_client.publish(std::move(event));
	return True();
}

Handle<Value> Susi::JS::Engine::RegisterConsumer(const v8::Arguments& args) {
	Locker locker{Susi::JS::engine->isolate};
	std::cout<<"in register consumer"<<std::endl;
	if (args.Length() < 2) return False();
	Handle<Value> callbackValue = args[1];
	if(callbackValue->IsFunction()){
		Handle<Value> topicValue = args[0];
		std::string topic{static_cast<std::string>(Susi::JS::Engine::convertFromJS(topicValue))};
		std::shared_ptr<Persistent<Function>> jsCallback{new Persistent<Function>(Handle<Function>::Cast(callbackValue))};
		Susi::Events::Consumer callback = [jsCallback](Susi::Events::SharedEventPtr event){
			Locker locker{Susi::JS::engine->isolate};
			std::cout<<"in callback"<<std::endl;
			Local<Function> func = Local<Function>::New(*jsCallback);
			Handle<Value> callbackArguments[1];
			callbackArguments[0] = Susi::JS::Engine::convertFromCPP(event->toAny());
			//TryCatch trycatch;
			auto res = func->Call(func,1,callbackArguments);
			if (res.IsEmpty()) {
				std::cout<<"got empty result"<<std::endl;
				/*Handle<Value> exception = trycatch.Exception();
				String::Utf8Value exception_str(exception);
				std::cout<<*exception_str<<std::endl;*/
			}
		};
		long id = Susi::JS::engine->susi_client.subscribe(topic,callback);
		return Number::New((double)id);
	}else{
		return False();
	}
	return True();
}

Handle<Value> Susi::JS::Engine::RegisterProcessor(const v8::Arguments& args) {
	Locker locker{Susi::JS::engine->isolate};
	if (args.Length() < 2) return False();
	Handle<Value> callbackValue = args[1];
	if(callbackValue->IsFunction()){
		Handle<Value> topicValue = args[0];
		std::string topic{Susi::JS::Engine::convertFromJS(topicValue).toString()};
		std::shared_ptr<Persistent<Function>> jsCallback{new Persistent<Function>(Handle<Function>::Cast(callbackValue))};
		Susi::Events::Processor callback = [jsCallback](Susi::Events::EventPtr event){
			Locker locker{Susi::JS::engine->isolate};
			Local<Function> func = Local<Function>::New(*jsCallback);
			Handle<Value> callbackArguments[1];
			callbackArguments[0] = Susi::JS::Engine::convertFromCPP(event->toAny());
			TryCatch trycatch;
			auto res = func->Call(func,1,callbackArguments);
			if (res.IsEmpty()) {
				Handle<Value> exception = trycatch.Exception();
				String::Utf8Value exception_str(exception);
				std::cout<<*exception_str<<std::endl;
			}
		};
		long id = Susi::JS::engine->susi_client.subscribe(topic,callback);
		return Number::New((double)id);
	}else{
		return False();
	}
	return True();
}

// void Susi::JSEngine::Unsubscribe(const v8::Arguments& args){
// 	Handle<Value> topicValue = args[0];
// 	Handle<Value> idValue = args[1];
// 	if(topicValue->IsString() && idValue->IsNumber()){
// 		std::string topic{Susi::convertFromJS(topicValue).toString()};
// 		long id{(long)Susi::convertFromJS(idValue).extract<double>()};
// 		Susi::unsubscribe(topic,id);
// 	}
// }
