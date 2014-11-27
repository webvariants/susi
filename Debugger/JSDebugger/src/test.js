/**
 * sample code
 */

//register master processor
susi.registerProcessor("master",function(event){
	event.payload = event.payload || {};
	event.payload.master = true;
	susi.publish({topic:"slave"},function(subevent){
		event.payload.slave = subevent.payload.slave;
		susi.ack(event);
	});
});

//register subroutine
susi.registerProcessor("slave",function(event){
	event.payload = event.payload || {};
	event.payload.slave = true;
	susi.ack(event);
});

//call master every second
susi.registerConsumer("heartbeat::one",function(event){
	susi.publish({topic: 'master'}, function(event){
		print("publish ready:", JSON.stringify(event));
	});
});
