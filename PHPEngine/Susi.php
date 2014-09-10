<?php
/*
 * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 * 
 * @author: Thomas Krause (thomas.krause@webvariants.de)
 */
require 'Event.php';


class Susi {

	// flags
	protected $debug     = false;
	protected $connected = false;

	protected $address = "localhost";
	protected $port    = "4000";
	protected $socket  = null;

	protected $consumer_callbacks  = array();
	protected $processor_callbacks = array();
	protected $finish_handlers     = array();
	
	public function __construct($address, $port, $debug = false) {
		$this->address = $address;
		$this->port    = $port;
		$this->debug   = $debug;
	}

	public function debug($msg) {
		if($this->debug) {
			echo $msg . "\n";
		}
	}

	public function registerConsumer($topic, $handler) {
		return $this->register("registerConsumer", $topic, $handler);
	}

	public function registerProcessor($topic, $handler) {
		return $this->register("registerProcessor", $topic, $handler);		
	}

	protected function register($type, $topic, $handler) {
		
		$register_id = intval(base_convert(uniqid(), 11, 10));		
		
		$callback = array (
			"id" => $register_id,
			"handler" => $handler
		);

		if($type == "registerConsumer") {
			if(array_key_exists($topic,$this->consumer_callbacks)){
				$this->consumer_callbacks[$topic][] = $callback;
			}else{
				$this->consumer_callbacks[$topic] = array($callback);

				// if susi is running, register first consumer with this topic
				if($this->connected) {
					fwrite($this->socket,json_encode(array("type" => "registerConsumer", "data" => $topic)));				
				}
			}
		} else {
			if(array_key_exists($topic,$this->processor_callbacks)){
				$this->processor_callbacks[$topic][] = $callback;
			}else{
				$this->processor_callbacks[$topic] = array($callback);
			
				// if susi is running, register first processor with this topic
				if($this->connected) {
					fwrite($this->socket,json_encode(array("type" => "registerProcessor", "data" => $topic)));				
				}
			}
		}

		return $register_id;
	}

	public function unregisterConsumer($register_id) {
		return $this->unregister("registerConsumer", $register_id);
	}

	public function unregisterProcessor($register_id) {
		return $this->unregister("registerProcessor", $register_id);	
	}

	protected function unregister($type, $register_id) {
		$found      = false;
		$tkey_found = null;

		if($type == "registerConsumer") {			
			foreach ($this->consumer_callbacks as $tkey => $topics) {
				foreach ($topics as $ckey => $callback) {
					if($callback["id"] == $register_id) {
						unset($this->consumer_callbacks[$tkey][$ckey]);
						$found = true;
						$tkey_found = $tkey;
						break;
					}
				}
			}	

			if($found === true && count($this->consumer_callbacks[$tkey_found]) == 0) {				
				$this->debug("unregisterConsumer: ".$tkey_found);
				if($this->connected) 
					fwrite($this->socket,json_encode(array("type" => "unregisterConsumer", "data" => $tkey_found)));
			}
		} else {			
			foreach ($this->processor_callbacks as $tkey => $topics) {
				foreach ($topics as $ckey => $callback) {
					if($callback["id"] == $register_id) {
						unset($this->processor_callbacks[$tkey][$ckey]);
						$found = true;
						$tkey_found = $tkey;
						break;
					}
				}
			}

			if($found === true && count($this->processor_callbacks[$tkey_found]) == 0) {
				$this->debug("unregisterProcessor: ".$tkey_found);
				if($this->connected)
					fwrite($this->socket,json_encode(array("type" => "unregisterProcessor", "data" => $tkey_found)));
			}
		}

		return $found;
	}

	public function ack($data = null) {
		fwrite($this->socket,json_encode(array("type" => "ack", "data" => $data)));
	}

	public function publish($event, $finish_handler = null) {

		if($finish_handler !== null) {
			$this->finish_handlers[$event->getStr_ID()] = $finish_handler;
		}

		$msg = array(
			"type" => "publish",
			"data" => array(
				"topic" => $event->getTopic(),
				"payload" => $event->getPayload(),
				"id" => $event->getID(),
				"headers" => $event->getHeaders()
			)
		);		

		fwrite($this->socket,json_encode($msg));
	}

	protected function handleIncome($data){
		$this->debug("PHPSusi handleIncome:\n" . $data);

		$msg = json_decode($data,true);

		if($msg["type"] === "consumerEvent"){

			$topic = $msg["data"]["topic"];

			if(array_key_exists($topic, $this->consumer_callbacks)){
				$consumer_callbacks = $this->consumer_callbacks[$topic];
				foreach ($consumer_callbacks as $callback) {
					try{						
						$callback["handler"]($msg);
					}catch(Exception $e){
						print($e);
					}
				}
			}
			
		}

		if($msg["type"] === "processorEvent"){		

			$topic = $msg["data"]["topic"];

			if(array_key_exists($topic, $this->processor_callbacks)){
				$processor_callbacks = $this->processor_callbacks[$topic];
				foreach ($processor_callbacks as $callback) {
					try{
						$callback["handler"]($msg);						
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

	protected function connect(){
		$this->socket = fsockopen($this->address, $this->port, $errno, $errstr, 5);

		if(!$this->socket) {
			echo "$errstr ($errno) \n";
			$this->connected = false;
			return false;
		}

		$this->connected = true;


		// register consumers
		foreach ($this->consumer_callbacks as $topic => $callbacks) {
			fwrite($this->socket,json_encode(array("type" => "registerConsumer", "data" => $topic)));			
		}

		// register processors
		foreach ($this->processor_callbacks as $topic => $callbacks) {
			fwrite($this->socket,json_encode(array("type" => "registerProcessor", "data" => $topic)));			
		}	

		return true;
	}
}
