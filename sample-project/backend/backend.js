susi.registerProcessor('chat-channel',function(evt){
	evt.payload = evt.payload.toUpperCase();
	susi.ack(evt);
});