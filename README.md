![Image of SUSI](http://webvariants.github.io/susi/resources/SUSI_Icon.svg)
# SUSI, a Universal System Interface

SUSI is an application framework to build interfaces for arbitrary systems.

It intends to enable even novice programmers to build robust asyncronous applications on both ends of the system: front-end and back-end.

## Getting started

### 1. Clone
First, clone the repo and its submodules from [Github](https://github.com/webvariants/susi)
```
git clone --recursive https://github.com/webvariants/susi
```

### 2. Build
SUSI's build process is *CMake* based. Therefore simply build with **cmake**.

After building, install the libraries and binaries and run ldconfig to update your shared library cache.
```
cd $SUSI
mkdir build
cd build
cmake ..
make -j4
sudo make install
sudo ldconfig
```

### 3. Start the SUSI core server
The main component of SUSI is its *core server* for handling the entire event dispatching and serves as a communication base for all other components.

You need to specify a valid TLS key / certificate pair to start the server.
To create a self-signed certificate run the following command:
```
openssl req -nodes -x509 -newkey rsa:2048 -keyout server_key.pem -out server_cert.pem -days 36500
```
Following, you can start the SUSI core server:
```
susi-core --key server_key.pem --cert server_cert.pem --port 4000
```
Now the SUSI core server is accepting TLS-connection on port 4000

#### Start one or more services
Once your core server has been started, you can start other components that connect to it.

For our first example lets start with susi-duktape - our server side javascript interpreter.

Create a Javascript source file, which looks like this:
```javascript
susi.registerProcessor('.*', function (evt) {
	console.debug('in processor');
	evt.payload = {};
	susi.ack(evt);
});

susi.registerProcessor('foo', function (evt) {
	console.debug('in foo proc 1');
	evt.payload.a = 'foo';
	susi.ack(evt);
});

susi.registerProcessor('foo', function (evt) {
	console.debug('in foo proc 2');
	evt.payload.b = 'bar';
	susi.dismiss(evt);
});

susi.registerProcessor('foo', function (evt) {
	console.debug('in foo proc 3 (should never be called)');
	evt.payload.c = 'baz';
	susi.ack(evt);
});

susi.registerConsumer('foo', function (evt) {
	console.log('consumer:', evt.payload);
});

susi.publish({ topic: 'foo' }, function (evt) {
	console.log('finish:', evt.payload);
});

```
Place this file somewhere in your filesystem with the name *susi-sample.js*.

You can use the same key/certificate pair you used to start the server, but you can create another pair:
```
openssl req -nodes -x509 -newkey rsa:2048 -keyout duktape_key.pem -out duktape_cert.pem -days 36500
```
Now its time to start *susi-duktape*.
```
susi-duktape --src susi-sample.js --addr localhost --port 4000 --key duktape_key.pem --cert duktape_cert.pem
> started Susi::duktape engine and loaded source.js
> 2015-08-25T09:35:31.934Z DBG susi-js: source.js:2: in processor
> 2015-08-25T09:35:31.934Z DBG susi-js: source.js:8: in foo proc 1
> 2015-08-25T09:35:31.934Z DBG susi-js: source.js:14: in foo proc 2
> 2015-08-25T09:35:31.934Z INF susi-js: consumer: {a:"foo",b:"bar"}
> 2015-08-25T09:35:31.935Z INF susi-js: finish: {a:"foo",b:"bar"}
```
If you see an output like this, everything's fine :)
If you take a look at the supplied JS sources, you can see how SUSI works.

### 4. Understand the code
There are 5 essential actions you need to know about:

* **registerProcessor()**
	* This attaches an active event handler to a specific topic.
	* All active event handlers run sequentially in the order of their declaration.
* **registerConsumer()**
	* This attaches a passive event handler to a specific topic
	* All passive event handlers will run after all active event handlers have finished.
* **publish()**
	* This publishes an event.
	* The event is firstly processed by all processors
	* After all processors finished, the consumers for this topic are called
* **ack()**
	* This needs to be called when a processor finished.
	* It tells susi, to continue with the event processing.
* **dismiss()**
	* This can also be called if a processor finished
	* It tells SUSI to stop the event processing -> no active handlers will be called after this
	* It will NOT stop passive handlers or the finish callback from being called

As you see in the example, we register four processors:

```javascript
susi.registerProcessor('.*', function (evt) {
	console.debug('in processor');
	evt.payload = {};
	susi.ack(evt);
});
```
This is the first processor. It takes a string/regex to specify the event topic the processor is interested in,
and a callback which is called. The first processor matches all events (topic: ".*") and ensures that the
event payload is an empty object. Notice that we call ack() at the end, to tell SUSI that the event can be processed by other processors now.


```javascript
susi.registerProcessor('foo', function (evt) {
	console.debug('in foo proc 1');
	evt.payload.a = 'foo';
	susi.ack(evt);
});
```
This is the second processor. It matches all events with the topic 'foo' and attaches the string 'foo' to the payload field 'a'. After this it acknowledges the event back to SUSI.

```javascript
susi.registerProcessor('foo', function (evt) {
	console.debug('in foo proc 2');
	evt.payload.b = 'bar';
	susi.dismiss(evt);
});
```
The third processor matches all events with the topic 'foo' as well.
Notice that we call dismiss() at the end of the callback. This prevents all later declared processors to be called.

```javascript
susi.registerProcessor('foo', function (evt) {
	console.debug('in foo proc 3 (should never be called)');
	evt.payload.c = 'baz';
	susi.ack(evt);
});
```
This is the fourth processor. It will never be called due to the dismiss() statement in the third processor.

```javascript
susi.registerConsumer('foo', function (evt) {
	console.log('consumer:', evt.payload);
});
```
Here we declare a Consumer / passive event handler.
It gets called after all processors, that are interested in this event, have finished.
In the callback we simply log the event payload to stdout.

```javascript
susi.publish({ topic: 'foo' }, function (evt) {
	console.log('finish:', evt.payload);
});
```
Now, after the setup of all those processors and consumers, we can finally publish an event!
publish() takes the event as first parameter. All events HAVE TO contain a topic field. Additionally they can have a payload field which can contain arbitrary data. As a second argument you can specify a finish callback. This is somewhat a one-time-consumer. It gets called after all processors for this event finished, but gets immediatly deleted afterwards.

### 5. Learn More about SUSI
* Documentation @ [susi.readme.io](http://susi.readme.io/).
* [Libraries](https://github.com/webvariants/susi/blob/experimental/LIBRARIES.md)
  * [Boost](https://github.com/boostorg/boost)
  * [OpenSSL](https://github.com/openssl/openssl)

### 6. Stay in contact
* Homepage: http://susi.io
* e-mail: tino[*dot*]rusch[*at*]webvariants[*dot*]de


SUSI is released under the [MIT License](https://github.com/webvariants/susi/blob/experimental/LICENSE.md).

*Many thanks to the whole SUSIProject Team and all [contributors](https://github.com/webvariants/susi/graphs/contributors).*

