/*global susi*/
/*global setTimeout*/

'use strict';

susi.registerConsumer('foo',function(evt){
    console.debug(evt);
});

setTimeout(function(){
    susi.publish({
        topic: 'foo',
        payload: 'test'
    },function(){});
},1000);
