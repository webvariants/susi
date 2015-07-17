var Susi = require('./susi');

var susi = new Susi('localhost', 4000, 'cert.pem', 'key.pem', function() {

    // register preprocessor for all events
    susi.registerProcessor('.*', function(evt) {
        evt.payload = evt.payload || {};
        susi.ack(evt);
    });

    // add 'foo' processor
    var processorId = susi.registerProcessor('foo', function(evt) {
        evt.payload.foo = 'bar';
        susi.ack(evt);
    });

    // add 'foo' consumer
    var consumerId = susi.registerConsumer('foo', function(evt) {
        console.log('consumer', JSON.stringify(evt.payload));
    });

    // publish 'foo'
    susi.publish({
        topic: 'foo'
    }, function(evt) {
        console.log('finish', JSON.stringify(evt.payload));
        console.log('-----------------------------------');

        // unregister processor 'foo'
        susi.unregisterProcessor(processorId);
        susi.publish({
            topic: 'foo'
        }, function(evt) {
            console.log('finish', JSON.stringify(evt.payload));
            console.log('-----------------------------------');

            // unregister consumer 'foo'
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
