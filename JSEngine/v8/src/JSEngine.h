/*
 * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 *
 * @authors: Tino Rusch (tino.rusch@webvariants.de), Christian Sonderfeld (christian.sonderfeld@webvariants.de)
 */

#ifndef __V8ENGINE__
#define __V8ENGINE__

#include "v8.h"
#include <iostream>
#include <functional>
#include <memory>
#include <fstream>
#include <streambuf>

#include "util/Channel.h"
#include "logger/Logger.h"
#include "util/Any.h"
#include "events/EventManager.h"
#include "apiserver/BasicApiClient.h"

namespace Susi {
namespace JS {
namespace V8 {



struct EngineToBridgeMessage {
	enum Type {PUBLISH, SUBSCRIBECONSUMER, SUBSCRIBEPROCESSOR, ACKNOWLEDGE, UNSUBSCRIBE};
	Type type;

	std::string topic;

	// For publish
	Susi::Util::Any payload;

	// For Acknowledge
	Susi::Events::Event event;
};

struct BridgeToEngineMessage {
	enum Type {CONSUMER, PROCESSOR, ACKNOWLEDGE};
	Type type;
	// For All Types
	Susi::Events::Event event;
};

using namespace v8;

class Engine{
protected:
	static Persistent<Context> setupContext() {
		// Create a template for the global object.
		Handle<ObjectTemplate> global{ObjectTemplate::New()};
		// Bind the global 'print' function to the C++ Print callback.
		global->Set(String::New("log"), FunctionTemplate::New(Susi::JS::V8::Engine::Log));
		global->Set(String::New("publish"), FunctionTemplate::New(Susi::JS::V8::Engine::Publish));
		global->Set(String::New("subscribeConsumer"), FunctionTemplate::New(Susi::JS::V8::Engine::subscribeConsumer));
		global->Set(String::New("subscribeProcessor"), FunctionTemplate::New(Susi::JS::V8::Engine::subscribeProcessor));

		return Context::New(NULL, global);
	}

	static Susi::Util::Any fromJS(Handle<Value> value) {
		Locker v8Locker;
		HandleScope scope;

	    Handle<Object> global = context->Global();

		Handle<Object> JSON = global->Get(String::New("JSON"))->ToObject();
	    Handle<Function> JSON_stringify = Handle<Function>::Cast(JSON->Get(String::New("stringify")));

	    Handle<Value> stringifiedValue = JSON_stringify->Call(JSON, 1, &value);
	    if(stringifiedValue->IsString()) {
	    	String::Utf8Value stringValue{stringifiedValue};
	    	return Susi::Util::Any::fromString(std::string(*stringValue));
	    }
	    return Susi::Util::Any{};
	}

	static Handle<Value> fromCPP(Susi::Util::Any value) {
		Locker v8Locker;
		HandleScope scope;

	    Handle<Object> global = Susi::JS::V8::Engine::context->Global();

		Handle<Object> JSON = global->Get(String::New("JSON"))->ToObject();
	    Handle<Function> JSON_parse = Handle<Function>::Cast(JSON->Get(String::New("parse")));

	    Handle<Value> valueHandle{String::New(value.toString().c_str())};

	    return scope.Close(JSON_parse->Call(JSON, 1, &valueHandle));
	}

	// Susi API Mapping

	static Handle<Value> Log(const Arguments& args) {
		Susi::Logger::log(fromJS(args[0]).toString());
		return Undefined();
	}

	static Handle<Value> Publish(const Arguments& args) {
		Susi::Logger::log("JSEngine - Publish: start");
		EngineToBridgeMessage message;
		message.type = EngineToBridgeMessage::PUBLISH;
		Susi::Logger::log("JSEngine - Publish: set obj");
		Susi::Util::Any obj = fromJS(args[0]);
		try {
			Susi::Logger::log("JSEngine - Publish: set topic");
			if (obj.isObject()) {
				message.topic = obj["topic"].toString();
				Susi::Logger::log("JSEngine - Publish: check payload");
				if(obj["payload"]) {
					message.payload = obj["payload"];
				}
			}
			else {
				message.topic = obj.toString();
			}
		}
		catch (Susi::Util::Any::WrongTypeException e) {
			Susi::Logger::log(e.what());
		}
		Susi::Logger::log("JSEngine - Publish: get timestamp");
		long cbID = std::chrono::system_clock::now().time_since_epoch().count();
		Susi::Logger::log("JSEngine - Publish: check callback");
		if(args.Length() > 1) {
			Handle<Function> cb = Handle<Function>::Cast(args[1]);
			if(!cb.IsEmpty()) {
				Susi::Logger::log("JSEngine - Publish: set callback");
				finishedCbs[cbID] = cb;
				message.payload["finishedCbID"] = cbID;
			}
		}
		Susi::Logger::log("JSEngine - Publish: send message to bridge");
		output_channel->put(message);
		return Undefined();
	}

	static Handle<Value> subscribeConsumer(const Arguments& args) {
		if(args.Length() < 2) {
			Susi::Logger::log("JSEngine - subscribeConsumer: wrong number of Arguments");
			return Undefined();
		}
		Locker v8Locker;
		HandleScope scope;

		Susi::Util::Any value = fromJS(args[0]);
		std::string topic = value;

		Handle<Function> callback = Handle<Function>::Cast(args[1]);

		if(callback.IsEmpty()) {
			Susi::Logger::log("JSEngine - subscribeConsumer: Topic: "+topic+" , failed - callback not found");
			return Undefined();
		}

		Susi::Logger::log("JSEngine - subscribeConsumer: Topic: "+topic);

		if(consumers.count(topic) == 0) {
			EngineToBridgeMessage message;
			message.type = EngineToBridgeMessage::SUBSCRIBECONSUMER;
			message.topic = topic;
			output_channel->put(message);
		}
		consumers[topic].push_back(callback);

		return Undefined();
	}

	static Handle<Value> subscribeProcessor(const Arguments& args) {
		if(args.Length() < 2) {
			Susi::Logger::log("JSEngine - subscribeProcessor: wrong number of Arguments");
			return Undefined();
		}
		Locker v8Locker;
		HandleScope scope;

		Susi::Util::Any value = fromJS(args[0]);
		std::string topic = value;

		Handle<Function> callback = Handle<Function>::Cast(args[1]);

		if(callback.IsEmpty()) {
			Susi::Logger::log("JSEngine - subscribeProcessor: Topic: "+topic+" , failed - callback not found");
			return Undefined();
		}

		Susi::Logger::log("JSEngine - subscribeProcessor: Topic: "+topic);

		if(processors.count(topic) == 0) {
			EngineToBridgeMessage message;
			message.type = EngineToBridgeMessage::SUBSCRIBEPROCESSOR;
			message.topic = topic;
			output_channel->put(message);
		}
		processors[topic].push_back(callback);

		return Undefined();
	}

	// First Executes a string within the current v8 context.
	// Waits for input from the input channel -> (Susi Event System)
	// Blocks until channel is destroyed
	bool run(Handle<String> source) {
		TryCatch try_catch;
		bool return_val = false;
		if (context.IsEmpty()) {
			Susi::Logger::log("JS::Engine.run: context is Empty");
			return_val = false;
	    }
	    else {
			context->Enter();
			Context::Scope context_scope{context};
			Handle<Script> script = Script::New(source, String::New("script"));
			if (script.IsEmpty()) {
				Susi::Logger::log("JS::Engine.run: Script is Empty");
				return_val = false;
			} else {
				Handle<Value> result = script->Run();
				if (result.IsEmpty()) {
					return_val = false;
			    }
			    else {
			    	return_val = true;
			    	Handle<Object> global{context->Global()};
					while(true) {
						try {
							auto message = input_channel->get();
							switch(message.type) {

								case BridgeToEngineMessage::CONSUMER: {
									Susi::Logger::log("JSEngine - run: Consumer");
									Handle<Value> evt = fromCPP(message.event.toAny());
									std::vector<Handle<Function>> callbacks{consumers.at(message.event.topic)};
									for(int i = 0; i< callbacks.size(); i++) {
										callbacks[i]->Call(global, 1, &evt);
									}
									break;
								}

								case BridgeToEngineMessage::PROCESSOR: {
									Susi::Logger::log("JSEngine - run: Processor");
									Handle<Value> evt = fromCPP(message.event.toAny());
									std::vector<Handle<Function>> callbacks{processors.at(message.event.topic)};
									for(int i = 0; i< callbacks.size(); i++) {
										evt = callbacks[i]->Call(global, 1, &evt);
									}
									message.event.setPayload(fromJS(evt)["payload"]);

									EngineToBridgeMessage ack;
									ack.event = message.event;
									ack.type = EngineToBridgeMessage::ACKNOWLEDGE;
									output_channel->put(ack);
									break;
								}

								case BridgeToEngineMessage::ACKNOWLEDGE: {
									Susi::Logger::log("JSEngine - run: Acknowledge");
									if(message.event.payload) {
										long cbID = message.event.payload["finishedCbID"];
										// TODO: delete message.event.payload["finishedCbID"];
										Handle<Value> evt = fromCPP(message.event.toAny());
										finishedCbs[cbID]->Call(global, 1, &evt);
										finishedCbs.erase(cbID);
									}
									break;
								}
							}
						}
						catch (Susi::Util::ChannelClosedException e) {
							break;
						}
					}
			    }
			}
			context->Exit();
	    }

		return return_val;
	}

public:
	static Persistent<Context> context;
	static std::map<std::string, std::vector<Handle<Function>>> consumers;
	static std::map<std::string, std::vector<Handle<Function>>> processors;
	static std::map<long, Handle<Function>> finishedCbs;
	static std::shared_ptr<Susi::Util::Channel<BridgeToEngineMessage>> input_channel;
	static std::shared_ptr<Susi::Util::Channel<EngineToBridgeMessage>> output_channel;

	HandleScope scope;


	Engine(std::shared_ptr<Susi::Util::Channel<BridgeToEngineMessage>> input_channel_,
		   std::shared_ptr<Susi::Util::Channel<EngineToBridgeMessage>> output_channel_)
	{
		input_channel = input_channel_;
		output_channel = output_channel_;

		if(context.IsEmpty()) {
			Locker v8Locker;
			HandleScope scope;
			TryCatch try_catch;
			context = Persistent<Context>(setupContext());
			if(try_catch.HasCaught()) {
				String::Utf8Value exception{try_catch.Exception()};
				Susi::Logger::error(std::string(*exception));
			}
		}
	}

	// This function will block the thread (because of run)
	bool run(std::string source) {
		Locker v8Locker;
		HandleScope scope;
		return this->run(String::New(source.c_str()));
	}

	// This function will block the thread (because of run)
	bool runFile(std::string source) {
		std::ifstream t(source);
		std::stringstream buffer;
		buffer << t.rdbuf();

		Locker v8Locker;
		HandleScope scope;
		return this->run(String::New(buffer.str().c_str()));
	}

	void stop() {
		input_channel->close();
	}

	~Engine() {
		context.Dispose();
	}
};

// define static variables of engine
Persistent<Context> Engine::context = Persistent<Context>();
std::map<std::string, std::vector<Handle<Function>>> Engine::consumers;
std::map<std::string, std::vector<Handle<Function>>> Engine::processors;
std::map<long, Handle<Function>> Engine::finishedCbs;
std::shared_ptr<Susi::Util::Channel<BridgeToEngineMessage>> Engine::input_channel;
std::shared_ptr<Susi::Util::Channel<EngineToBridgeMessage>> Engine::output_channel;


class EngineBridge : public Susi::Api::BasicApiClient {
protected:
	std::shared_ptr<Susi::Util::Channel<EngineToBridgeMessage>> input_channel;
	std::shared_ptr<Susi::Util::Channel<BridgeToEngineMessage>> output_channel;

	bool running;
	std::thread t;
	std::map<std::string, long> topicIDs;

	void onAck(Susi::Events::Event & event) {
		Susi::Logger::log("JSEngineBridge - onAck");
		BridgeToEngineMessage message;
		message.type = BridgeToEngineMessage::ACKNOWLEDGE;
		message.event = event;
		output_channel->put(message);
	}

	void onConsumerEvent(Susi::Events::Event & event) {
		Susi::Logger::log("JSEngineBridge - onConsumerEvent");
		BridgeToEngineMessage message;
		message.type = BridgeToEngineMessage::CONSUMER;
		message.event = event;
		output_channel->put(message);
	}

	void onProcessorEvent(Susi::Events::Event & event) {
		Susi::Logger::log("JSEngineBridge - onProcessorEvent");
		BridgeToEngineMessage message;
		message.type = BridgeToEngineMessage::PROCESSOR;
		message.event = event;
		output_channel->put(message);
	}

public:
	EngineBridge(std::shared_ptr<Susi::Util::Channel<EngineToBridgeMessage>> input_channel_,
		std::shared_ptr<Susi::Util::Channel<BridgeToEngineMessage>> output_channel_,
		std::string address = "[::1]:4000")
		: BasicApiClient{address},
		running{true} {
		input_channel = input_channel_;
		output_channel = output_channel_;
	}

	void start() {
		t = std::move(std::thread{[this]() {
			while(true) {
				try {
					auto message = this->input_channel->get();
					switch(message.type) {
						case EngineToBridgeMessage::PUBLISH:
						{
							Susi::Logger::log("JSEngineBridge - start: Publish");
							auto evt = Susi::Events::Event(message.topic);
							if(!message.payload.isNull()) {
								evt.setPayload(message.payload);
							}
							sendPublish(evt);
							break;
						}
						case EngineToBridgeMessage::SUBSCRIBECONSUMER:
						{
							Susi::Logger::log("JSEngineBridge - start: RegisterConsumer");
							sendRegisterConsumer(message.topic);
							break;
						}
						case EngineToBridgeMessage::SUBSCRIBEPROCESSOR:
						{
							Susi::Logger::log("JSEngineBridge - start: RegisterProcessor");
							sendRegisterProcessor(message.topic);
							break;
						}
						case EngineToBridgeMessage::UNSUBSCRIBE:
						{
							Susi::Logger::log("JSEngineBridge: UNSUBSCRIBE NOT YET IMPLEMENTED!");
						}
						case EngineToBridgeMessage::ACKNOWLEDGE:
						{
							Susi::Logger::log("JSEngineBridge - start: Acknowledge");
							sendAck(message.event);
						}
						default: {
							Susi::Logger::log("JSEngineBridge - start: Message Type Unknown");
							break;
						}
					}
				} catch(Susi::Util::ChannelClosedException e) {
					return;
				}
			}
		}});
	}

	void stop() {
		input_channel->close();
		if(t.joinable()) {
			t.join();
		}
	}
};

}

}
}

#endif // __V8ENGINE__
