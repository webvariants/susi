susi.registerProcessor("duktapetest",function(event){
	log("duktapeTest processor callback: "+JSON.stringify(event));
	event.payload = {foo:'bar'};
	susi.ack(event);
});

susi.registerConsumer("duktapetest",function(event){
	log("duktapeTest consumer callback: "+JSON.stringify(event));
});

susi.publish({topic:"duktapetest"},function(event){
	log("duktapeTest finish callback: "+JSON.stringify(event));
});