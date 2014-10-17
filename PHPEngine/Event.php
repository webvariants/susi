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

 // Event structur	
 //  [data] => Array
 //        (
 //            [headers] => Array()        // Array of Arrays, used for dynamic 
 //            [id] => xxxxxxxxxxxxxxxxxx  // unique id
 //            [payload] =>                // payload 
 //            [sessionid] =>              // sessionID
 //            [topic] => heartbeat::five  // Topic 
 //        )
 //  [type] => processorEvent            // Eventtype ("consumerEvent", "processorEvent", "ack", "status")

class Event {

	protected $id        = 0;
	protected $str_id    = "";

	protected $topic     = "";
	protected $authlevel = 3;
	protected $payload   = array();
	protected $headers   = array();	
	protected $sessionID = "";
	protected $type      = "";

	protected $error     = false;
	protected $errMsg    = "";


	public function __construct($topic = "", $payload = array(), $headers = array(), $authlevel = 3) {

		$this->str_id    = base_convert(uniqid(), 11, 10);
		$this->id        = intval($this->str_id);

		$this->topic     = $topic;
		$this->authlevel = $authlevel;
		$this->payload   = $payload;
		$this->headers   = $headers;
	}


	// getter
	public function getID() {
		return $this->id;
	}

	public function getStr_ID() {
		return $this->str_id;
	}

	public function getTopic() {
		return $this->topic;
	}

	public function getAuthlevel() {
		return $this->authlevel;
	}

	public function getHeaders() {
		return $this->headers;
	}

	public function getPayload() {
		return $this->payload;
	}

	public function getSessionID() {
		return $this->sessionID;
	}

	public function getType() {
		return $this->type;
	}

	public function getError() {
		return $this->error;
	}

	public function getErrorMsg() {
		return $this->errMsg;
	}

	// setter
	public function setTopic($topic = "") {
		$this->topic = $topic;
	}

	public function setHeader($header = array()) {
		$this->headers[] = $header;
	}

	public function setHeaders($headers = array()) {
		$this->headers = $headers;
	}

	public function setPayload($payload = array()) {
		$this->payload = $payload;
	}

	public function setSessionID($sessionID = "") {
		$this->sessionID = $sessionID;
	}

	public function setType($type = "") {
		$this->type = $type;
	}

	public function fromString($msg) {
		$msg = json_decode($msg, true);

		$this->type  = $msg["type"];
		$this->error = array_key_exists("error", $msg) ? $msg["error"] : false;

		// if error is true, data will contain a string 
		if(array_key_exists('data', $msg) && $this->error === false) {

			$data = $msg['data'];

			$this->str_id    = array_key_exists('id', $data)        ? "" + $data["id"]   : "";
			$this->topic     = array_key_exists('topic', $data)     ? $data["topic"]     : "";
			$this->authlevel = array_key_exists('authlevel', $data) ? $data["authlevel"] : 3;
			$this->headers   = array_key_exists('headers', $data)   ? $data["headers"]   : array();
			$this->payload   = array_key_exists('payload', $data)   ? $data["payload"]   : array();
			$this->sessionID = array_key_exists('sessionid', $data) ? $data["sessionid"] : "";

		} else {
			$this->str_id = "";
		}

		$this->id    = intval($this->str_id);
		

		if($this->error === true && array_key_exists("data", $msg)) {
			$this->errMsg = $msg["data"];
		} else {
			$this->errMsg = "";
		}
	}

	public function toString() {
		$msg = array(
			"type" => $this->type,
			"data" => $this->getData()
		);

		return json_encode($msg);
	}

	public function getData() {
		return $data = array(
			"topic"     => $this->topic,
			"authlevel" => $this->authlevel,
			"payload"   => $this->payload,
			"id"        => $this->id,
			"sessionid" => $this->sessionID,
			"headers"   => $this->headers			
		);				
	}
}
