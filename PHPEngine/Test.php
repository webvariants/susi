<?php

define("_DIR", dirname(__FILE__));

require _DIR.'/Susi.php';
require _DIR.'/IONContainer.php';
require _DIR.'/config.php';

class SusiTest extends Susi {

	protected function connect(){		
		$this->socket = fsockopen($this->address, $this->port, $errno, $errstr, 5);

		if(!$this->socket) {
			echo "$errstr ($errno)<br />\n";
			$this->connected = false;
			return false;
		}

		$this->connected = true;


		// register consumers
		foreach ($this->consumer_callbacks as $topic => $callbacks) {
			echo "Register Consumer:". $topic . "\n";
			fwrite($this->socket,json_encode(array("type" => "registerConsumer", "data" => $topic)));			
		}

		// register processors
		foreach ($this->processor_callbacks as $topic => $callbacks) {
			echo "Register Processor:". $topic . "\n";
			fwrite($this->socket,json_encode(array("type" => "registerProcessor", "data" => $topic)));			
		}	

		
		echo "----------------------\n";

		$pub_test = array (
			"testdata" => "foo",			
		);

		$this->publish("test_controller", $pub_test);		

		return true;
	}
}

$susi      = new SusiTest($CONFIG["SUSI_ADDR"], $CONFIG["SUSI_PORT"], true);
$container = new IONContainer($CONFIG);

echo "Run Test: \n";

$reg_1_id = $susi->registerProcessor("test_controller",

	// callback
	function(&$event) use($susi,$container) {

		if(isset($event["data"]["payload"])) {
			$event["data"]["payload"]["test1"] = "ok";
		}

	}
);

$reg_2_id = $susi->registerProcessor("test_controller",

	// callback
	function(&$event) use($susi,$container) {
		
		if(isset($event["data"]["payload"])) {
			$event["data"]["payload"]["test2"] = "ok";
		}

	}
);

$reg_3_id = $susi->registerConsumer("test_controller",

	// callback
	function(&$event) use($susi,$container) {
		
		print_r($event);
	}
);

$susi->run();