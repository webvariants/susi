<?php

class Susi {	
	private $address = "localhost";
	private $port    = "4000";
	private $socket  = null;

	private $resisters = array();
	private $consumer_handlers  = array();
	private $processor_handlers = array();
	private $finish_handlers    = array();

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

		// will be used on connect, only uniq topics will be registered
		$this->resisters[$topic] = $msg;

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
		$msg = array(
			"type" => "ack",
			"data" => $data
		);

		fwrite($this->socket,json_encode($msg));
	}

	public function publish($topic, $payload = null, $finish_handler = null) {		
		
		// id must be an long
		$str_id = base_convert(uniqid(), 11, 10);		
		$int_id = intval($str_id);

		if($finish_handler !== null) {
			$this->finish_handlers[$str_id] = $finish_handler;
		}

		$msg = array(
			"type" => "publish",
			"data" => array(
				"topic" => $topic,
				"payload" => $payload,
				"id" => $int_id
			)
		);		

		fwrite($this->socket,json_encode($msg));
	}


	protected function handleIncome($data){
		echo "PHPSusi handleIncome:" . $data . "\n";

		$msg = json_decode($data,true);

		if($msg["type"] === "consumerEvent"){

			$topic = $msg["data"]["topic"];

			if(array_key_exists($topic, $this->consumer_handlers)){
				$consumer_handlers = $this->consumer_handlers[$topic];
				foreach ($consumer_handlers as $handler) {
					try{						
						$handler($msg);
					}catch(Exception $e){
						print($e);
					}
				}
			}
			
		}

		if($msg["type"] === "processorEvent"){		

			$topic = $msg["data"]["topic"];

			if(array_key_exists($topic, $this->processor_handlers)){
				$processor_handlers = $this->processor_handlers[$topic];
				foreach ($processor_handlers as $handler) {
					try{
						$handler($msg);						
					}catch(Exception $e){
						print($e);
					}
				}
				$this->ack($msg["data"]);
			}
		}

		if($msg["type"] === "ack"){
			// convert to string
			$id = "" + $msg["data"]["id"];

			if(array_key_exists($id, $this->finish_handlers)){
				$finish_handler = $this->finish_handlers[$id];
				try{				
					$finish_handler($msg);
					// delete callback from memory
					unset($this->finish_handlers[$id]);					
				}catch(Exception $e){
					print($e);
				}
			}
		}

		if($msg["type"] === "status"){
			if($msg["error"] == true) {
				$error_msg = $msg["data"];
				echo "server error:".$error_msg."\n";
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

		$this->publish("test_controller", $pub_test);

		//{"type" : "publish" , "data" : { "topic" : "php_controller" , "payload" : { "controller" : "foo" , "action" : "bar" } } }

		return true;
	}
}