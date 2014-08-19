<?php

class Susi {	
	private $address = "localhost";
	private $port    = "4000";
	private $socket  = null;

	private $resisters = array();
	private $consumer_handlers  = array();
	private $processor_handlers = array();
	private $globs     = array();
	
	public function __construct($address, $port) {
		$this->address = $address;
		$this->port    = $port;
	}

	public function registerConsumer($topic, $handler) {
		$this->register("registerConsumer", $topic, $handler);
	}

	public function registerProcessor($topic, $handler) {
		$this->register("registerProcessor", $topic, $handler);		
	}

	public function register($type, $topic, $handler) {
		$msg = array (
			"type" => $type,
			"data" => $topic
		);

		$this->resisters[] = $msg;	

		if($type == "registerConsumer") {
			if(array_key_exists($topic,$this->consumer_handlers)){
				$this->consumer_handlers[$topic][] = $handler;
			}else{
				$this->consumer_handlers[$topic] = array($handler);
			}
		} else {
			if(array_key_exists($topic,$this->processor_handlers)){
				$this->processor_handlers[$topic][] = $handler;
			}else{
				$this->processor_handlers[$topic] = array($handler);
			}
		}		
	}

	public function ack($data = null) {
		echo "PHPSusi ack: \n";
		//print_r($data);

		$msg = array(
			"type" => "ack",
			"data" => $data
		);

		fwrite($this->socket,json_encode($msg));
	}

	public function publish($topic, $payload = null) {
		$msg = array(
			"type" => "publish",
			"data" => array(
				"topic" => $topic,
				"payload" => $payload
			)
		);		
	
		fwrite($this->socket,json_encode($msg));
	}


	protected function handleIncome($data){
		echo "PHPSusi handleIncome:" . $data . "\n";

		$msg = json_decode($data,true);

		if($msg["type"] === "consumerEvent"){
			echo "PHPSusi handleIncome consumerEvent: \n";

			print_r($msg);

			$topic = $msg["data"]["topic"];

			if(array_key_exists($topic, $this->consumer_handlers)){
				$consumer_handlers = $this->consumer_handlers[$topic];
				foreach ($consumer_handlers as $handler) {
					try{
						echo "handler found!!!";
						$handler($msg);
					}catch(Exception $e){
						print($e);
					}
				}
			}
			
		}

		if($msg["type"] === "processorEvent"){
			echo "PHPSusi handleIncome processorEvent: \n";

			print_r($msg);

			$topic = $msg["data"]["topic"];

			if(array_key_exists($topic, $this->processor_handlers)){
				$processor_handlers = $this->processor_handlers[$topic];
				foreach ($processor_handlers as $handler) {
					try{
						echo "handler found!!!";
						$handler($msg);
					}catch(Exception $e){
						print($e);
					}
				}
			}
		}
	}

	public function run(){
		while(true){
			if(!$this->connect()){
				echo "Connect \n";
				sleep(1);
				continue;
			}
	
			$data = "";
			$openingBraces = 0;
			$closingBraces = 0;
	
			while(($buff = fgets($this->socket,1024))){				
				$data .= $buff;
				echo "RUN:".$data ."\n";
				TAG:
				for($i=0;$i<strlen($data);$i++){
					if ($data[$i]=="{") $openingBraces++;
					if ($data[$i]=="}") $closingBraces++;
					if ($openingBraces == $closingBraces && $openingBraces > 0) {
						$packet = substr($data,0,$i+1);
						$data = substr($data,$i+1);
						$this->handleIncome($packet);
						$openingBraces = 0;
						$closingBraces = 0;
						goto TAG;
					}
				}
			}
		}
	}

	private function connect(){
		$this->socket = fsockopen($this->address, $this->port, $errno, $errstr, 5);

		if(!$this->socket) {
			echo "$errstr ($errno)<br />\n";
			return false;
		}


		foreach ($this->resisters as $msg) {
			echo "register:". json_encode($msg) . "\n";
			fwrite($this->socket,json_encode($msg));
		}		

		$pub_test = array (
			"pub_controller" => "lala",			
		);

		$this->publish("php_controller", $pub_test);

		//{"type" : "publish" , "data" : { "topic" : "php_controller" , "payload" : { "controller" : "foo" , "action" : "bar" } } }

		return true;
	}
}