#include "susi/SusiClient.h"
#include "susi/TinyJS.h"
#include "susi/TinyJS_Functions.h"
#include "susi/TinyJS_MathFunctions.h"

#include <fstream>
#include <streambuf>

class SusiTinyJS {

public:
	SusiTinyJS(Susi::SusiClient & susi, std::string script): susi_{susi},script_{script} {
		js_ = std::make_shared<CTinyJS>();
		registerFunctions(js_.get());
		registerMathFunctions(js_.get());
	}
	~SusiTinyJS(){
	}
	void start(){
		try{
			js_->addNative("function log(arg)",[](CScriptVar *v, void *self){
				std::cout<<"jslog: "<<v->getParameter("arg")->getString()<<std::endl;
			},(void*)this);
			js_->addNative("function _publish(event)",[](CScriptVar *v, void *self){
				BSON::Value event = BSON::Value::fromJSON(v->getParameter("event")->getString());
				auto * me = (SusiTinyJS*)self;
				try{
					std::string topic = event["topic"];
					auto payload = event["payload"];
					auto id = event["id"];
					me->susi_.publish(topic,payload,[me,id](const BSON::Value & event){
						BSON::Value evt{event};
						evt["id"] = id;
						std::string cmd = "_processAck('"+evt.toJSON()+"');";
						try{
							me->js_->execute(cmd);
						}catch(CScriptException *e){
							std::cerr<<cmd<<std::endl;
							std::cerr<<e->text.c_str()<<std::endl;
						}
					});
				}catch(const std::exception & e){
					me->js_->execute("_processAck('"+event.toJSON()+"');");
				}
			},(void*)this);
			js_->addNative("function _registerProcessor(topic)",[](CScriptVar *v, void *self){
				std::cout<<"registerProcessor "<<v->getParameter("topic")->getString()<<std::endl;
			},(void*)this);
			js_->addNative("function _registerConsumer(topic)",[](CScriptVar *v, void *self){
				std::cout<<"registerConsumer "<<v->getParameter("topic")->getString()<<std::endl;
			},(void*)this);

	  		js_->execute(susijs);
			std::ifstream t(script_);
			std::string code((std::istreambuf_iterator<char>(t)),
			                 std::istreambuf_iterator<char>());
	  		js_->execute(code.c_str());
		}catch(CScriptException *e){
			std::cerr<<e->text.c_str()<<std::endl;
		}
	}

	Susi::SusiClient & susi_;
	std::shared_ptr<CTinyJS> js_;	
protected:
	std::string script_;
	const std::string susijs = R"SUSIJS(

	var susi = {  
		_consumerCallbacks: {}, 
		_processorCallbacks: {}, 
		_publishCallbacks: {},
		
		registerConsumer: function(topic,callback) { 
			var consumers = this._consumerCallbacks[topic] || []; 
			var id = this._genID(); 
			consumers.push({callback: callback, id: id}); 
			this._consumerCallbacks[topic] = consumers; 
			if(consumers.length == 1){ 
				_registerConsumer(topic); 
			} 
			return id; 
		},

		registerProcessor: function(topic,callback) { 
			var processors = this._processorCallbacks[topic] || []; 
			var id = this._genID(); 
			processors.push({callback: callback, id: id}); 
			this._processorCallbacks[topic] = processors; 
			if(processors.length == 1){ 
				_registerProcessor(topic); 
			} 
			return id; 
		},

		unregisterConsumer: function(id){ 
			for(var topic in this._consumerCallbacks){ 
				var callbacks = this._consumerCallbacks[topic]; 
				for(var i=0; i<callbacks.length; i++){ 
					if(callbacks[i].id === id){ 
						callbacks.splice(i,1); 
						if(callbacks.length === 0){ 
							delete this._consumerCallbacks[topic]; 
						} 
						return true; 
					} 
				} 
			} 
			return false; 
		},

		unregisterProcessor: function(id){ 
			for(var topic in this._processorCallbacks){ 
				var callbacks = this._processorCallbacks[topic]; 
				for(var i=0; i<callbacks.length; i++){ 
					if(callbacks[i].id === id){ 
						callbacks.splice(i,1); 
						if(callbacks.length === 0){ 
							delete this._processorCallbacks[topic]; 
						} 
						return true; 
					} 
				} 
			} 
			return false; 
		},

		publish: function(event,callback) { 
			if(event.id === undefined){ 
				event.id = ''+this._genID(); 
			} 
			if(callback !== undefined) { 
				this._publishCallbacks[event.id] = callback; 
			} 
			_publish(JSON.stringify(event,null)); 
		},

		ack: function(event){ 
			_ack(JSON.stringify(event)); 
		},

		//used by js to interact with c++ part
		_processConsumerEvent: function(event,topic){ 
			event = eval(event); 
			var consumers = this._consumerCallbacks[topic] || []; 
			if(consumers.length > 0){ 
				for (var i = consumers.length - 1; i >= 0; i--) { 
					consumers[i].callback(event); 
				} 
			} 
		},

		_processProcessorEvent: function(event,topic){ 
			event = eval(event); 
			var processors = this._processorCallbacks[topic] || []; 
			if(processors.length > 0){ 
				for (var i = processors.length - 1; i >= 0; i--) { 
					processors[i].callback(event); 
				} 
			} 
			susi.ack(JSON.stringify(event));
		},

		_processAck: function(event){ 
			event = eval(event);
			var cb = this._publishCallbacks[event.id]; 
			if(cb !== undefined) {
				cb(event); 
				this._publishCallbacks[event.id] = undefined; 
			}
		},

		_genID: function(){ 
			return Math.randInt(0,100000000000); 
		} 
	}; 

	//called by c++ part
	function _processConsumerEvent(event,topic){ 
		susi._processConsumerEvent(event,topic); 
	}

	function _processProcessorEvent(event,topic){ 
		susi._processProcessorEvent(event,topic) 
	}

	function _processAck(event){ 
		susi._processAck(event); 
	}

	)SUSIJS";
};


