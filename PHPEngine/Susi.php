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

	public function calcHash($topic, $authlevel) {
		return hash('ripemd160', $topic . '_' . $authlevel);
	}

	public function registerConsumer($topic, $handler, $authlevel = 3, $name = "") {
		return $this->register("registerConsumer", $topic, $handler, $authlevel, $name);
	}

	public function registerProcessor($topic, $handler, $authlevel = 3, $name = "") {
		return $this->register("registerProcessor", $topic, $handler, $authlevel, $name);		
	}

	protected function register($type, $topic, $handler, $authlevel, $name) {
		
		$register_id = intval(base_convert(uniqid(), 11, 10));
		$hash = $this->calcHash($topic, $authlevel);
		
		$callback = array (
			"id" => $register_id,
			"handler" => $handler,
			/* temps for connect and reinit on connect */
			"topic" => $topic,
			"authlevel" => $authlevel,
			"name" => $name
		);

		if($type == "registerConsumer") {
			if(array_key_exists($hash,$this->consumer_callbacks)){
				$this->consumer_callbacks[$hash][] = $callback;
			}else{
				$this->consumer_callbacks[$hash] = array($callback);

				// if susi is running, register first consumer with this hash(topic,authlevel)
				if($this->connected) {
					//fwrite($this->socket,json_encode(array("type" => "registerConsumer", "data" => $topic)));
					fwrite($this->socket,json_encode(array("type" => "registerConsumer", "data" => array(
						"topic" => $topic,
						"authlevel" => $authlevel,
						"name" => $name
						))));
				}
			}
		} else {
			if(array_key_exists($hash,$this->processor_callbacks)){
				$this->processor_callbacks[$hash][] = $callback;
			}else{
				$this->processor_callbacks[$hash] = array($callback);
			
				// if susi is running, register first processor with this hash(topic,authlevel)
				if($this->connected) {
					//fwrite($this->socket,json_encode(array("type" => "registerProcessor", "data" => $topic)));					
					fwrite($this->socket,json_encode(array("type" => "registerProcessor", "data" => array(
						"topic" => $topic,
						"authlevel" => $callback["authlevel"],
						"name" => $callback["name"]
						))));
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
		$hash_found = null;
		$topic      = "";
		$authlevel  = 3;

		if($type == "registerConsumer") {			
			foreach ($this->consumer_callbacks as $hash => $callbacks) {
				foreach ($callbacks as $ckey => $callback) {
					if($callback["id"] == $register_id) {
						$topic     = $callback["topic"];
						$authlevel = $callback["authlevel"];
						unset($this->consumer_callbacks[$hash][$ckey]);
						$found = true;
						$hash_found = $hash;
						break;
					}
				}
			}	

			if($found === true && count($this->consumer_callbacks[$hash_found]) == 0) {				
				$this->debug("unregisterConsumer: ".$hash_found);

				unset($this->consumer_callbacks[$hash_found]);
				if($this->connected) 
					fwrite($this->socket,json_encode(array("type" => "unregisterConsumer", "data" => array(
							"topic" => $topic,
							"authlevel" => $authlevel
						))));
			}
		} else {			
			foreach ($this->processor_callbacks as $hash => $topics) {
				foreach ($topics as $ckey => $callback) {
					if($callback["id"] == $register_id) {
						$topic     = $callback["topic"];
						$authlevel = $callback["authlevel"];
						unset($this->processor_callbacks[$hash][$ckey]);
						$found = true;
						$hash_found = $hash;
						break;
					}
				}
			}

			if($found === true && count($this->processor_callbacks[$hash_found]) == 0) {
				$this->debug("unregisterProcessor: ".$hash_found);

				unset($this->processor_callbacks[$hash_found]);
				if($this->connected)
					fwrite($this->socket,json_encode(array("type" => "unregisterProcessor", "data" => array(
							"topic" => $topic,
							"authlevel" => $authlevel
						))));
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

		$event->setType("publish");
		
		fwrite($this->socket,$event->toString());
	}

	protected function handleIncome($data){
		$this->debug("PHPSusi handleIncome:\n" . $this->indent($data));

		$evt = new Event();
		$evt->fromString($data);
		
		if($evt->getType() === "consumerEvent"){

			$topic     = $evt->getTopic();
			$authlevel = $evt->getAuthlevel();
			$hash      = $this->calcHash($topic, $authlevel);

			if(array_key_exists($hash, $this->consumer_callbacks)){
				$consumer_callbacks = $this->consumer_callbacks[$hash];
				foreach ($consumer_callbacks as $callback) {
					try{						
						$callback["handler"]($evt);
					}catch(Exception $e){
						print($e);
					}
				}
			}
			
		}

		if($evt->getType() === "processorEvent"){		

			$topic     = $evt->getTopic();
			$authlevel = $evt->getAuthlevel();
			$hash      = $this->calcHash($topic, $authlevel);
			
			if(array_key_exists($hash, $this->processor_callbacks)){
				$processor_callbacks = $this->processor_callbacks[$hash];
				foreach ($processor_callbacks as $callback) {
					try{
						$callback["handler"]($evt);						
					}catch(Exception $e){
						print($e);
					}
				}
				$this->ack($evt->getData());
			}
		}

		if($evt->getType() === "ack"){
			// ID as string
		 	$id = $evt->getStr_ID();

			if(array_key_exists($id, $this->finish_handlers)){
				$finish_handler = $this->finish_handlers[$id];
				try{				
					$finish_handler($evt);
					// delete callback from memory
					unset($this->finish_handlers[$id]);					
				}catch(Exception $e){
					print($e);
				}
			}
		}

		if($evt->getType() === "status"){
			if($evt->getError() == true) {
				$error_msg = $evt->getErrorMsg();
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
		$this->socket = fsockopen($this->address, $this->port, $errno, $errstr, 5000);

		if(!$this->socket) {
			echo "$errstr ($errno) \n";
			$this->connected = false;
			return false;
		}

		$this->connected = true;


		// register consumers
		foreach ($this->consumer_callbacks as $hash => $callbacks) {
			$callback = $callbacks[0];		

			//fwrite($this->socket,json_encode(array("type" => "registerConsumer", "data" => $topic)));
			fwrite($this->socket,json_encode(array("type" => "registerConsumer", "data" => array(
						"topic" => $callback["topic"],
						"authlevel" => $callback["authlevel"],
						"name" => $callback["name"]
						))));
		}

		// register processors
		foreach ($this->processor_callbacks as $hash => $callbacks) {
			$callback = $callbacks[0];
			//fwrite($this->socket,json_encode(array("type" => "registerProcessor", "data" => $topic)));	
			fwrite($this->socket,json_encode(array("type" => "registerProcessor", "data" => array(
						"topic" => $callback["topic"],
						"authlevel" => $callback["authlevel"],
						"name" => $callback["name"]
						))));
		}	

		return true;
	}

	// helper function for console
	/**
	 * Indents a flat JSON string to make it more human-readable.
	 *
	 * @param string $json The original JSON string to process.
	 *
	 * @return string Indented version of the original JSON string.
	 */
	function indent($json) {

	    $result      = '';
	    $pos         = 0;
	    $strLen      = strlen($json);
	    $indentStr   = '  ';
	    $newLine     = "\n";
	    $prevChar    = '';
	    $outOfQuotes = true;

	    for ($i=0; $i<=$strLen; $i++) {

	        // Grab the next character in the string.
	        $char = substr($json, $i, 1);

	        // Are we inside a quoted string?
	        if ($char == '"' && $prevChar != '\\') {
	            $outOfQuotes = !$outOfQuotes;

	        // If this character is the end of an element,
	        // output a new line and indent the next line.
	        } else if(($char == '}' || $char == ']') && $outOfQuotes) {
	            $result .= $newLine;
	            $pos --;
	            for ($j=0; $j<$pos; $j++) {
	                $result .= $indentStr;
	            }
	        }

	        // Add the character to the result string.
	        $result .= $char;

	        // If the last character was the beginning of an element,
	        // output a new line and indent the next line.
	        if (($char == ',' || $char == '{' || $char == '[') && $outOfQuotes) {
	            $result .= $newLine;
	            if ($char == '{' || $char == '[') {
	                $pos ++;
	            }

	            for ($j = 0; $j < $pos; $j++) {
	                $result .= $indentStr;
	            }
	        }

	        $prevChar = $char;
	    }

	    return $result;
	}

}