#include "susi/DuktapeEngine.h"

std::string Susi::Duktape::susiJS = R"SUSIJS(

var susi = {
    _consumerCallbacks: [],
    _processorCallbacks: [],

    _processorTopicCounter: {},
    _consumerTopicCounter: {},

    _publishCallbacks: {},
    _processorProcesses: {},

    _processed: [],

    registerConsumer: function(topic,callback) {
        var id = this._genID();
        this._consumerCallbacks.push({topic: topic, callback: callback, id: id});
        var count = this._consumerTopicCounter[topic] || 0;
        count++;
        this._consumerTopicCounter[topic] = count;
        if(count === 1){
            _registerConsumer(topic);
        }
        return id;
    },

    registerProcessor: function(topic,callback) {
        var id = this._genID();
        this._processorCallbacks.push({topic: topic, callback: callback, id: id});
        var count = this._processorTopicCounter[topic] || 0;
        count++;
        this._processorTopicCounter[topic] = count;
        if(count == 1){
            _registerProcessor(topic);
        }
        return id;
    },

    unregisterConsumer: function(id){
        for(var i=0;i<this._consumerCallbacks;i++){
            if(this._consumerCallbacks[i].id === id){
                var topic = this._consumerCallbacks[i].topic;
                this._consumerCallbacks.splice(i,1);
                var count = this._consumerTopicCounter[topic];
                count--;
                if(count === 0){
                    delete(this._consumerTopicCounter[topic]);
                    _unregisterConsumer(topic);
                }else{
                    this._consumerTopicCounter[topic] = count;
                }
                return true;
            }
        }
        return false;
    },

    unregisterProcessor: function(id){
        for(var i=0;i<this._processorCallbacks;i++){
            if(this._processorCallbacks[i].id === id){
                var topic = this._processorCallbacks[i].topic;
                this._processorCallbacks.splice(i,1);
                var count = this._processorTopicCounter[topic];
                count--;
                if(count === 0){
                    delete(this._processorTopicCounter[topic]);
                    _unregisterProcessor(topic);
                }else{
                    this._processorTopicCounter[topic] = count;
                }
                return true;
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
        _publish(JSON.stringify(event));
    },

    ack: function(event){
        var process = this._processorProcesses[event.id];
        if(process.next >= process.processors.length){
            delete this._processorProcesses[event.id];
            _ack(JSON.stringify(event));
        }else{
            process.next++;
            process.processors[process.next-1](event);
        }
    },

    dismiss: function(event){
        delete(this._processorProcesses[event.id]);
        _dismiss(JSON.stringify(event));
    },

    //used by js to interact with c++ part
    _processConsumerEvent: function(event){
        event = JSON.parse(event);
        for(var i=0;i<this._consumerCallbacks.length;i++){
            if(event.topic.match(this._consumerCallbacks[i].topic)){
                this._consumerCallbacks[i].callback(event);
            }
        }
    },

    _processProcessorEvent: function(event,topic){
        event = JSON.parse(event);
        if(this._processed.indexOf(event.id) !== -1){
            _ack(JSON.stringify(event));
            return;
        }
        this._processed.push(event.id);
        if(this._processed.length > 64){
            this._processed.splice(0,1);
        }
        Duktape.fin(event,function(event){
            susi.ack(event);
        });
        var process = {
            processors: [],
            next: 0
        };

        for (var i = 0; i<this._processorCallbacks.length; i++) {
            if(event.topic.match(this._processorCallbacks[i].topic)){
                process.processors.push(this._processorCallbacks[i].callback);
            }
        }

        this._processorProcesses[event.id] = process;
        this.ack(event);
        Duktape.gc();
    },

    _processAck: function(event){
        event = JSON.parse(event);
        var cb = this._publishCallbacks[event.id];
        if(cb !== undefined) {
            cb(event);
            delete this._publishCallbacks[event.id];
        }
    },

    _genID: function(){
        return Math.floor(Math.random()*1000000000000);
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

var duktapeLogger = new Duktape.Logger('susi-js');
duktapeLogger.l = 0;

var console = {
    _prepareArguments: function(args){
        var newArgs = [];
        for(var i=0;i<args.length;i++){
            if(typeof args[i] === 'object'){
                newArgs.push(Duktape.enc('jx',args[i]));
            }else{
                newArgs.push(args[i]);
            }
        }
        return newArgs;
    },
    _getLine: function(){
        var e = new Error(arguments);
        return e.stack.split('\n')[3].trim().split(' ')[1];
    },
    log: function(){
        duktapeLogger.info.apply(duktapeLogger,this._prepareArguments(arguments));
    },
    debug: function(){
        duktapeLogger.n = 'susi-js: '+this._getLine();
        duktapeLogger.debug.apply(duktapeLogger,this._prepareArguments(arguments));
        duktapeLogger.n = 'susi-js';
    },
    error: function(){
        duktapeLogger.n = 'susi-js: '+this._getLine();
        duktapeLogger.error.apply(duktapeLogger,this._prepareArguments(arguments));
        duktapeLogger.n = 'susi-js';
    }
};

var TimeoutManager = {
    timeoutFunctions: [],
    add: function(cb, millis){
        this.timeoutFunctions.push({
            deadline: Date.now()+millis,
            cb: cb
        });
    },
    check: function(){
        var now = Date.now();
        for(var i=this.timeoutFunctions.length-1;i>=0;i--){
            if(this.timeoutFunctions[i].deadline <= now){
                this.timeoutFunctions[i].cb();
                this.timeoutFunctions.splice(i,1);
            }
        }
    }
};

function setTimeout(cb,millis){
    TimeoutManager.add(cb,millis);
    _setTimeout(millis);
}

function _checkTimeouts(){
    TimeoutManager.check();
}


)SUSIJS";
