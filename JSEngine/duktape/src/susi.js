/*
 * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 * 
 * @author: Tino Rusch (tino.rusch@webvariants.de)
 */

var susi = {
	_consumerCallbacks: {},
	_processorCallbacks: {},
	_publishCallbacks: {},

	registerConsumer: function(topic,callback) {
		var consumers = this._consumerCallbacks[topic] || [];
		consumers.push(callback);
		this._consumerCallbacks[topic] = consumers;
		if(consumers.length == 1){
			_registerConsumer(topic);
		}
	},

	registerProcessor: function(topic,callback) {
		var processors = this._processorCallbacks[topic] || [];
		processors.push(callback);
		this._processorCallbacks[topic] = processors;
		if(processors.length == 1){
			_registerProcessor(topic);
		}
	},

	publish: function(event,callback) {
		if(event.id === undefined){
			event.id = ""+Math.floor(Math.random()*1000000000000);
		}
		if(callback !== undefined) {
			this._publishCallbacks[event.id] = callback;
		}
		_publish(JSON.stringify(event));
	},

	_processConsumerEvent: function(event){
		event = JSON.parse(event);
		var consumers = this._consumerCallbacks[event.topic] || [];
		if(consumers.length > 0){
			for (var i = consumers.length - 1; i >= 0; i--) {
				consumers[i](event);
			}
		}
	},

	_processProcessorEvent: function(event){
		event = JSON.parse(event);
		Duktape.fin(event,function(event){
			_ack(JSON.stringify(event));
		});
		var processors = this._processorCallbacks[event.topic] || [];
		if(processors.length > 0){
			for (var i = processors.length - 1; i >= 0; i--) {
				processors[i](event);
			}
		}
		Duktape.gc();
	},

	_processAck: function(event){
		event = JSON.parse(event);
		var cb = this._publishCallbacks[event.id];
		if(cb !== undefined) {
			delete this._publishCallbacks[event.id];
		}
		cb(event);
	}

}

function _processConsumerEvent(event){
	susi._processConsumerEvent(event);
}
function _processProcessorEvent(event){
	susi._processProcessorEvent(event)
}
function _processAck(event){
	susi._processAck(event);
}

/**
 * sample code:
 */

susi.registerProcessor("bar",function(event){
	event.payload = event.payload || {};
	event.payload.bar = true;
});

susi.registerProcessor("foo",function(event){
	event.payload = event.payload || {};
	event.payload.foo = true;
	susi.publish({topic:"bar"},function(subevent){
		event.payload.bar = subevent.payload.bar;
	});
});

susi.registerConsumer("heartbeat::one",function(event){
	susi.publish({topic: 'foo'}, function(event){
		print("publish ready: ",JSON.stringify(event));
	});
});


