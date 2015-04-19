var Susi = function (cb) {
	this.consumers = {};
	this.publishCallbacks = {};
	this.ws = new WebSocket("ws://"+window.location.host+"/ws");
	var self = this;
	this.ws.onopen = function(){
		setInterval(function(){
			self.ws.send(JSON.stringify({
				type: "publish",
				data: {
					topic: "session::update"
				}
			}));
		},2000);
		if(cb){
			cb(self);
		}
	};
	this.ws.onmessage = function(msg){
		self._handleIncome(msg);
	};
};


Susi.prototype.registerConsumer = function(topic,cb) {
	var consumers = this.consumers[topic] || [];
	consumers.push(cb);
	if(consumers.length === 1){
		this.ws.send(JSON.stringify({
			type: "registerConsumer",
			data: {
				topic: topic
			}
		}));
	}
	this.consumers[topic] = consumers;
};

Susi.prototype.publish = function(topic,payload,cb) {
	var id = ''+Date.now();
	this.publishCallbacks[id] = cb;
	this.ws.send(JSON.stringify({
		type: 'publish',
		data: {
			topic: topic,
			payload: payload,
			id: id
		},
	}));
};

Susi.prototype._handleIncome = function(msg){
	var packet = JSON.parse(msg.data);
	if( packet.type === 'consumerEvent'){
		var consumers = this.consumers[packet.data.topic];
		for (var i = consumers.length - 1; i >= 0; i--) {
			consumers[i](packet.data);
		};
	}
	if (packet.type === 'ack'){
		if(packet.data.id && this.publishCallbacks[packet.data.id]){
			this.publishCallbacks[packet.data.id](packet.data);
			delete this.publishCallbacks[packet.data.id];
		}
	}
};
