var fs = require('fs');
var tls = require('tls');
var net = require('net');
var crypto = require('crypto');
var JSONStream = require('JSONStream')


function Susi(host,port,cert,key,cb){
    var _finishCallbacks = {};
    var _consumers = [];
    var _processors = [];

    var _consumerTopicCounter = {};
    var _processorTopicCounter = {};

    var _publishProcesses = {};

    var keyPem = fs.readFileSync(key, encoding='ascii');
    var certPem = fs.readFileSync(cert, encoding='ascii');
    var options = {
        key:keyPem, 
        cert:certPem,
        rejectUnauthorized: false
    };
    var _socket = null;

    var self = this;

    var reregister = function(){
        for(topic in _consumerTopicCounter){
            _socket.write(JSON.stringify({type: 'registerConsumer',data:{topic: topic}}));
        }
        for(topic in _processorTopicCounter){
            _socket.write(JSON.stringify({type: 'registerProcessor',data:{topic: topic}}));
        }
    };

    var startSuccess = function(){
        _socket.setEncoding('utf-8');
        var _buffer = '';
        reregister();
        _socket.on('data',function(data){
            _buffer += data;
            while(_buffer.indexOf('\n') !== -1){
                var msg = _buffer.substring(0, _buffer.indexOf('\n'))
                _buffer = _buffer.substring(_buffer.indexOf('\n')+1)
                var doc = JSON.parse(msg);
                if((doc.type == 'ack' || doc.type == 'dismiss') && _finishCallbacks[doc.data.id]){
                    _finishCallbacks[doc.data.id](doc.data);
                    delete _finishCallbacks[doc.data.id];
                }else if(doc.type == 'consumerEvent'){
                    for(var i=0;i<_consumers.length;i++){
                        if(doc.data.topic.match(_consumers[i].topic)){
                            _consumers[i].callback(doc.data);
                        }
                    }
                }else if(doc.type == 'processorEvent'){
                    var publishProcess = {
                        next: 0,
                        processors: []
                    };
                    for(var i=0;i<_processors.length;i++){
                        if(doc.data.topic.match(_processors[i].topic)){
                           publishProcess.processors.push(_processors[i].callback);
                        }
                    }
                    _publishProcesses[doc.data.id] = publishProcess;
                    self.ack(doc.data);
                }
            }
        });
        if(cb){
            cb(self)
        }
    }

    var initSocket = function(){
        _socket = tls.connect(4000,"localhost", options, startSuccess);
        _socket.on('error',function(){
            console.log('error connecting susi...');
            setTimeout(initSocket,500);
        }); 
        _socket.on('close',function(){
            console.log('error connecting susi...');
            setTimeout(initSocket,500);
        });        
    };
    initSocket();

    var generateId = function(){
        return crypto.randomBytes(16).toString('hex');
    };

    self.publish = function(evt,finishCallback){
        if(!typeof evt.topic === 'string'){
            return false;
        }
        evt.id = evt.id || generateId();
        _socket.write(JSON.stringify({type:'publish',data: evt}));
        if(finishCallback){
            _finishCallbacks[evt.id] = finishCallback;
        }
        return true;
    };

    self.ack = function(evt){
        var publishProcess = _publishProcesses[evt.id];
        if(!publishProcess){
            return;
        }
        var next = publishProcess.next;
        var processors = publishProcess.processors;
        if(next < processors.length){
            publishProcess.next++;
            processors[next](evt);
        }else{
            _socket.write(JSON.stringify({type:'ack',data: evt}));
            delete(_publishProcesses[evt.id]);
        }
    };

    self.dismiss = function(evt){
        if(_publishProcesses[evt.id]){
            _socket.write(JSON.stringify({type:'dismiss',data: evt}));
            delete(_publishProcesses[evt.id]);
        }
    };

    self.registerConsumer = function(topic,callback){
        var obj = {
            topic: topic,
            callback: callback,
            id : generateId()
        }
        _consumers.push(obj);
        _consumerTopicCounter[topic] = _consumerTopicCounter[topic] || 0;
        _consumerTopicCounter[topic]++;
        if(_consumerTopicCounter[topic] == 1){
            _socket.write(JSON.stringify({type: 'registerConsumer',data:{topic: topic}}));
        }
        return obj.id;
    };

    self.registerProcessor = function(topic,callback){
        var obj = {
            topic: topic,
            callback: callback,
            id : generateId()
        }
        _processors.push(obj);
        _processorTopicCounter[topic] = _processorTopicCounter[topic] || 0;
        _processorTopicCounter[topic]++;
        if(_processorTopicCounter[topic] == 1){
            _socket.write(JSON.stringify({type: 'registerProcessor',data:{topic: topic}}));
        }
        return obj.id;
    };

    self.unregisterConsumer = function(id){
        for(var i=0;i<_consumers.length;i++){
            if(_consumers[i].id == id){
                var consumer = _consumers.splice(i,1);
                _consumerTopicCounter[consumer.topic]--;
                if(_consumerTopicCounter[consumer.topic] === 0){
                    _socket.write(JSON.stringify({type: 'unregisterConsumer', data: {topic: consumer.topic}}));
                    delete(_consumerTopicCounter[consumer.topic]);
                }
                break;
            }
        }
    };

    self.unregisterProcessor = function(id){
        for(var i=0;i<_processors.length;i++){
            if(_processors[i].id == id){
                var processor = _processors.splice(i,1);
                _processorTopicCounter[processor.topic]--;
                if(_processorTopicCounter[processor.topic] === 0){
                    _socket.write(JSON.stringify({type: 'unregisterProcessor', data: {topic: processor.topic}}));
                    delete(_processorTopicCounter[processor.topic]);
                }
                break;
            }
        }
    };
}

module.exports = Susi