var users = {};

susi.registerProcessor('register-key',function(evt){
	log("got register key");
	users[evt.sessionid] = users[evt.sessionid] || {name: 'anonymous', key: ''};
	users[evt.sessionid].key = evt.payload;
	susi.ack(evt);
});

susi.registerProcessor('register-name',function(evt){
	log("got register name");
	users[evt.sessionid] = users[evt.sessionid] || {name: 'anonymous', key: ''};
	users[evt.sessionid].name = evt.payload;
	susi.ack(evt);
});

susi.registerProcessor('get-users',function(evt){
	log("got get-users");
	evt.payload = users;
	susi.ack(evt);
});

susi.registerConsumer('session::died',function(evt){
	log("remove user "+evt.payload);
	delete users[evt.payload];
});