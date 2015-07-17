var Susi = require('./susi');

var susi = new Susi('localhost', 4000, 'cert.pem', 'key.pem', function() {

    susi.registerProcessor('.*', function(evt) {
        evt.payload = evt.payload || {};
        susi.ack(evt);
    });

    var processorId = susi.registerProcessor('foo', function(evt) {
        evt.payload.foo = 'bar';
        susi.ack(evt);
    });

    var consumerId = susi.registerConsumer('foo', function(evt) {
        console.log('consumer', JSON.stringify(evt.payload));
    });

    susi.publish({
        topic: 'foo'
    }, function(evt) {
        console.log('finish', JSON.stringify(evt.payload));
        console.log('-----------------------------------');

        susi.unregisterProcessor(processorId);
        susi.publish({
            topic: 'foo'
        }, function(evt) {
            console.log('finish', JSON.stringify(evt.payload));
            console.log('-----------------------------------');

            susi.unregisterConsumer(consumerId);
            susi.publish({
                topic: 'foo'
            }, function(evt) {
                console.log('finish', JSON.stringify(evt.payload));
                console.log('-----------------------------------');
            });
        });
    });

});
