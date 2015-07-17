var fs = require('fs');
var tls = require('tls');
var net = require('net');
var crypto = require('crypto');
var JSONStream = require('JSONStream')


function Susi(host,port,cert,key,cb){
    this._finishCallbacks = {};
    this._consumers = [];
    this._processors = [];

    this._consumerTopicCounter = {};
    this._processorTopicCounter = {};

    this._publishProcesses = {};

    var keyPem = fs.readFileSync(key, encoding='ascii');
    var certPem = fs.readFileSync(cert, encoding='ascii');
    var options = {
        key:keyPem, 
        cert:certPem,
        rejectUnauthorized: false
    };
    var self = this;

    var reregister = function(){
        for(topic in self._consumerTopicCounter){
            self._socket.write(JSON.stringify({type: 'registerConsumer',data:{topic: topic}}));
        }
        for(topic in self._processorTopicCounter){
            self._socket.write(JSON.stringify({type: 'registerProcessor',data:{topic: topic}}));
        }
    };

    self._startSuccess = function(){
        self._socket.setEncoding('utf-8');
        self._buffer = '';
        reregister();
        self._socket.on('data',function(data){
            self._buffer += data;
            while(self._buffer.indexOf('\n') !== -1){
                var msg = self._buffer.substring(0, self._buffer.indexOf('\n'))
                self._buffer = self._buffer.substring(self._buffer.indexOf('\n')+1)
                var doc = JSON.parse(msg);
                if((doc.type == 'ack' || doc.type == 'dismiss') && self._finishCallbacks[doc.data.id]){
                    self._finishCallbacks[doc.data.id](doc.data);
                    delete self._finishCallbacks[doc.data.id];
                }else if(doc.type == 'consumerEvent'){
                    for(var i=0;i<self._consumers.length;i++){
                        if(doc.data.topic.match(self._consumers[i].topic)){
                            self._consumers[i].callback(doc.data);
                        }
                    }
                }else if(doc.type == 'processorEvent'){
                    var publishProcess = {
                        next: 0,
                        processors: []
                    };
                    for(var i=0;i<self._processors.length;i++){
                        if(doc.data.topic.match(self._processors[i].topic)){
                           publishProcess.processors.push(self._processors[i].callback);
                        }
                    }
                    self._publishProcesses[doc.data.id] = publishProcess;
                    self.ack(doc.data);
                }
            }
        });
        if(cb){
            cb(self)
        }
    }

    var initSocket = function(){
        self._socket = tls.connect(4000,"localhost", options, self._startSuccess);
        self._socket.on('error',function(){
            console.log('error connecting susi...');
            setTimeout(initSocket,500);
        }); 
        self._socket.on('close',function(){
            console.log('error connecting susi...');
            setTimeout(initSocket,500);
        });        
    };
    initSocket();

    self._generateId = function(){
        return crypto.randomBytes(16).toString('hex');
    };

    self.publish = function(evt,finishCallback){
        if(!typeof evt.topic === 'string'){
            return false;
        }
        evt.id = evt.id || this._generateId();
        console.log(evt);
        self._socket.write(JSON.stringify({type:'publish',data: evt}));
        if(finishCallback){
            self._finishCallbacks[evt.id] = finishCallback;
        }
        return true;
    };

    self.ack = function(evt){
        var publishProcess = self._publishProcesses[evt.id];
        if(!publishProcess){
            return;
        }
        var next = publishProcess.next;
        var processors = publishProcess.processors;
        if(next < processors.length){
            publishProcess.next++;
            processors[next](evt);
        }else{
            self._socket.write(JSON.stringify({type:'ack',data: evt}));
            delete(self._publishProcesses[evt.id]);
        }
    };

    self.dismiss = function(evt){
        if(self._publishProcesses[evt.id]){
            self._socket.write(JSON.stringify({type:'dismiss',data: evt}));
            delete(self._publishProcesses[evt.id]);
        }
    };

    self.registerConsumer = function(topic,callback){
        var obj = {
            topic: topic,
            callback: callback,
            id : this._generateId()
        }
        this._consumers.push(obj);
        this._consumerTopicCounter[topic] = this._consumerTopicCounter[topic] || 0;
        this._consumerTopicCounter[topic]++;
        if(this._consumerTopicCounter[topic] == 1){
            self._socket.write(JSON.stringify({type: 'registerConsumer',data:{topic: topic}}));
        }
        return obj.id;
    };

    self.registerProcessor = function(topic,callback){
        var obj = {
            topic: topic,
            callback: callback,
            id : this._generateId()
        }
        this._processors.push(obj);
        this._processorTopicCounter[topic] = this._processorTopicCounter[topic] || 0;
        this._processorTopicCounter[topic]++;
        if(this._processorTopicCounter[topic] == 1){
            self._socket.write(JSON.stringify({type: 'registerProcessor',data:{topic: topic}}));
        }
        return obj.id;
    };

    self.unregisterConsumer = function(id){
        for(var i=0;i<this._consumers.length;i++){
            if(this._consumers[i].id == id){
                var consumer = this._consumers.splice(i,1);
                this._consumerTopicCounter[consumer.topic]--;
                if(this._consumerTopicCounter[consumer.topic] === 0){
                    self._socket.write(JSON.stringify({type: 'unregisterConsumer', data: {topic: consumer.topic}}));
                    delete(this._consumerTopicCounter[consumer.topic]);
                }
                break;
            }
        }
    };

    self.unregisterProcessor = function(id){
        for(var i=0;i<this._processors.length;i++){
            if(this._processors[i].id == id){
                var processor = this._processors.splice(i,1);
                this._processorTopicCounter[processor.topic]--;
                if(this._processorTopicCounter[processor.topic] === 0){
                    self._socket.write(JSON.stringify({type: 'unregisterProcessor', data: {topic: processor.topic}}));
                    delete(this._processorTopicCounter[processor.topic]);
                }
                break;
            }
        }
    };
}

module.exports = Susi