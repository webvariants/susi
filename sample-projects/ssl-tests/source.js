/* jshint strict: false */
/* global susi */


susi.registerProcessor('foo', function (evt) {
	console.debug('in foo proc');
	evt.payload = 'foo';
	susi.ack(evt);
});

susi.registerConsumer('foo', function (evt) {
	console.debug('consumer:', evt.payload);
});

susi.registerConsumer('heartbeat::one', function(){
	susi.publish({ topic: 'foo' }, function (evt) {
		console.debug('finish:', evt.payload);
	});
});


setTimeout(function(){
	console.log('foooooooo!');
},1000);
setTimeout(function(){
        console.log('foooooooo!');
},1000);
setTimeout(function(){
        console.log('foooooooo!');
},1000);
setTimeout(function(){
        console.log('foooooooo!');
},1000);


