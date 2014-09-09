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

class Event {

	protected $id        = 0;
	protected $str_id    = "";

	protected $topic     = "";
	protected $payload   = array();
	protected $headers   = array();	
	protected $sessionID = "";


	public function __construct($topic, $payload, $headers = array()) {

		$this->str_id    = base_convert(uniqid(), 11, 10);
		$this->id        = intval($this->str_id);

		$this->topic     = $topic;
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

	public function getHeaders() {
		return $this->headers;
	}

	public function getPayload() {
		return $this->payload;
	}

	public function getSessionID() {
		return $this->sessionID;
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
}
