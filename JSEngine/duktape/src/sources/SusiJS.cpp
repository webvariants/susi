#include "DuktapeEngine.h"

std::string Duktape::susiJS = " \
var susi = {  \
	_consumerCallbacks: {}, \
	_processorCallbacks: {}, \
	_publishCallbacks: {}, \
	registerConsumer: function(topic,callback) { \
		var consumers = this._consumerCallbacks[topic] || []; \
		var id = this._genID(); \
		consumers.push({callback: callback, id: id}); \
		this._consumerCallbacks[topic] = consumers; \
		if(consumers.length == 1){ \
			_registerConsumer(topic); \
		} \
		return id; \
	}, \
	registerProcessor: function(topic,callback) { \
		var processors = this._processorCallbacks[topic] || []; \
		var id = this._genID(); \
		processors.push({callback: callback, id: id}); \
		this._processorCallbacks[topic] = processors; \
		if(processors.length == 1){ \
			_registerProcessor(topic); \
		} \
		return id; \
	}, \
	unregisterConsumer: function(id){ \
		for(var topic in this._consumerCallbacks){ \
			var callbacks = this._consumerCallbacks[topic]; \
			for(var i=0; i<callbacks.length; i++){ \
				if(callbacks[i].id === id){ \
					callbacks.splice(i,1); \
					if(callbacks.length === 0){ \
						delete this._consumerCallbacks[topic]; \
					} \
					return true; \
				} \
			} \
		} \
		return false; \
	}, \
	unregisterProcessor: function(id){ \
		for(var topic in this._processorCallbacks){ \
			var callbacks = this._processorCallbacks[topic]; \
			for(var i=0; i<callbacks.length; i++){ \
				if(callbacks[i].id === id){ \
					callbacks.splice(i,1); \
					if(callbacks.length === 0){ \
						delete this._processorCallbacks[topic]; \
					} \
					return true; \
				} \
			} \
		} \
		return false; \
	}, \
	publish: function(event,callback) { \
		if(event.id === undefined){ \
			event.id = ''+this._genID(); \
		} \
		if(callback !== undefined) { \
			this._publishCallbacks[event.id] = callback; \
		} \
		_publish(JSON.stringify(event)); \
	}, \
	ack: function(event){ \
		_ack(JSON.stringify(event)); \
	}, \
	_processConsumerEvent: function(event,topic){ \
		event = JSON.parse(event); \
		var consumers = this._consumerCallbacks[topic] || []; \
		if(consumers.length > 0){ \
			for (var i = consumers.length - 1; i >= 0; i--) { \
				consumers[i].callback(event); \
			} \
		} \
	}, \
	_processProcessorEvent: function(event,topic){ \
		event = JSON.parse(event); \
		Duktape.fin(event,function(event){ \
			susi.ack(event); \
		}); \
		var processors = this._processorCallbacks[topic] || []; \
		if(processors.length > 0){ \
			for (var i = processors.length - 1; i >= 0; i--) { \
				processors[i].callback(event); \
			} \
		} \
		Duktape.gc(); \
	}, \
	_processAck: function(event){ \
		event = JSON.parse(event); \
		var cb = this._publishCallbacks[event.id]; \
		if(cb !== undefined) { \
			delete this._publishCallbacks[event.id]; \
		} \
		cb(event); \
	}, \
	_genID: function(){ \
		return Math.floor(Math.random()*1000000000000); \
	} \
}; \
function _processConsumerEvent(event,topic){ \
	susi._processConsumerEvent(event,topic); \
} \
function _processProcessorEvent(event,topic){ \
	susi._processProcessorEvent(event,topic) \
} \
function _processAck(event){ \
	susi._processAck(event); \
}";
