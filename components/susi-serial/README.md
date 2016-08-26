# Susi-Serial

This component allows the communication, through a serial connection, to the susi server.  
You can open several serial ports by configuring an array with nested objects.  
Every object has there personal settings for this port.

##### Example Configuration file

```javascript
{
	"addr": "localhost",
	"port": 4000,
	"cert": "/home/user/susi/sample-projects/ssl-tests/cert1.pem",
	"key": "/home/user/susi/sample-projects/ssl-tests/key1.pem",
	"ports" : [
		{
			"id" : "ardu1",
			"port" : "/dev/ttyACM2",
			"baudrate" : 119200,
			"char_size": 8,
			"parity": "ODD"
		},
		{
			"id" : "ardu2",
			"port" : "/dev/ttyACM5",
			"baudrate" : 9600,
			"char_size": 5,
			"parity": "even"
		}
	]
}

```

---

##### Events

There a are two events which are fired.  
One is the ```serial::data``` event. It transports the, on the interface, received string to the server.  
The other one is the ```serial::write```. This send the given string to the interface.  

---

##### Valid Values

Below are tables of valid values for the port configuration.

|Speed|  
|---|---|---|---|  
|0|50|75|110|  
|135|150|200|300|  
|600|1200|1800|2400|  
|4800|9600|119200|38400|  
|57600|115200|230400|  

|Char size|
|---|---|---|---|
|8|7|6|5|

|Parity|
|---|---|---|
|ODD|EVEN|NONE|

---
