#include "DuktapeEngine.h"

std::string Duktape::susiJS = " \
var susi = {  \
	_consumerCallbacks: {}, \
	_processorCallbacks: {}, \
	_publishCallbacks: {}, \
	registerConsumer: function(topic,callback) { \
		var consumers = this._consumerCallbacks[topic] || []; \
		consumers.push(callback); \
		this._consumerCallbacks[topic] = consumers; \
		if(consumers.length == 1){ \
			_registerConsumer(topic); \
		} \
	}, \
	registerProcessor: function(topic,callback) { \
		var processors = this._processorCallbacks[topic] || []; \
		processors.push(callback); \
		this._processorCallbacks[topic] = processors; \
		if(processors.length == 1){ \
		_registerProcessor(topic); \
		} \
	}, \
	publish: function(event,callback) { \
		if(event.id === undefined){ \
			event.id = ''+Math.floor(Math.random()*1000000000000); \
		} \
		if(callback !== undefined) { \
			this._publishCallbacks[event.id] = callback; \
		} \
		_publish(JSON.stringify(event)); \
	}, \
	ack: function(event){ \
		_ack(JSON.stringify(event)); \
	}, \
	_processConsumerEvent: function(event){ \
		event = JSON.parse(event); \
		var consumers = this._consumerCallbacks[event.topic] || []; \
		if(consumers.length > 0){ \
			for (var i = consumers.length - 1; i >= 0; i--) { \
				consumers[i](event); \
			} \
		} \
	}, \
	_processProcessorEvent: function(event){ \
		event = JSON.parse(event); \
		Duktape.fin(event,function(event){ \
			susi.ack(event); \
		}); \
		var processors = this._processorCallbacks[event.topic] || []; \
		if(processors.length > 0){ \
			for (var i = processors.length - 1; i >= 0; i--) { \
				processors[i](event); \
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
	} \
}; \
function _processConsumerEvent(event){ \
	susi._processConsumerEvent(event); \
} \
function _processProcessorEvent(event){ \
	susi._processProcessorEvent(event) \
} \
function _processAck(event){ \
	susi._processAck(event); \
}";
