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

//call master every second five times
var counter = 0;
var id = susi.registerConsumer("heartbeat::one",function(event){
	if(++counter == 10){
		susi.unregisterConsumer(id);
	}
	susi.publish({topic: 'master'}, function(event){
		log("publish ready: "+JSON.stringify(event));
	});
});


