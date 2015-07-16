var fs = require('fs');
var tls = require('tls');
var net = require('net');
var crypto = require('crypto');
var JSONStream = require('JSONStream')


function Susi(host,port,cert,key,cb){
    var keyPem = fs.readFileSync(key, encoding='ascii');
    var certPem = fs.readFileSync(cert, encoding='ascii');
    var options = {
        key:keyPem, 
        cert:certPem,
        rejectUnauthorized: false
    };
    this.finishCallbacks = {};
    var self = this;
    self.socket = tls.connect(4000,"localhost", options, function() {
        self.socket.setEncoding('utf-8');
        self.socket.on('data',function(data){
            var buffer = "";
            if (data.indexOf('\n') < 0) {
                buffer += data;
            } else {
                var msg = buffer + data.substring(0, data.indexOf('\n'));
                buffer = data.substring(data.indexOf('\n') + 1);
                var doc = JSON.parse(msg);
                if(doc.type == 'ack' && self.finishCallbacks[doc.data.id]){
                    self.finishCallbacks[doc.data.id](doc.data);
                    delete self.finishCallbacks[doc.data.id];
                }
            }
        });
        if(cb){
            cb(self)
        }
    })

    self.publish = function(evt,finishCallback){
        evt.id = evt.id || crypto.randomBytes(32).toString('hex');
        self.socket.write(JSON.stringify({type:'publish',data: evt}));
        if(finishCallback){
            self.finishCallbacks[evt.id] = finishCallback;
        }
    };
}

var susi = new Susi("localhost",4000,"cert.pem","key.pem",function(){
    susi.publish({topic: 'foobar'}, function(evt){
        console.log(JSON.stringify(evt));
    })
});


module.exports = Susi