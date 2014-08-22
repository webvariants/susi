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
		$this->publish("test_start", array());				

		return true;
	}
}

$susi      = new SusiTest($CONFIG["SUSI_ADDR"], $CONFIG["SUSI_PORT"], false);
$container = new IONContainer($CONFIG);

echo "Run Test: \n";

// INFO 
// pass $event in processor and consumer handler as reference !!!

// Register Processor & Consumer
$susi->registerProcessor("test_start",

	// callback
	function(&$event) use($susi,$container) {
		echo "Test: \n";

		echo "Test Register: \n";
		$p_id_1 = $susi->registerProcessor("test",
			function(&$event) use($susi,$container) {
				echo "Test Processor1 Callback \n";
				$event["data"]["payload"]["processor1"] = "ok";
				$event["data"]["headers"][] = array("foo" => "bar");
			}
		);
		$p_id_2 = $susi->registerProcessor("test",
			function(&$event) use($susi,$container) {
				echo "Test Processor2 Callback \n";
				$event["data"]["payload"]["processor2"] = "ok";
				$event["data"]["headers"][] = array("john" => "doe");
			}
		);
		$c_id = $susi->registerConsumer("test",
			function(&$event) use($susi,$container) {
				echo "Test Consumer Callback >>> ";

				if(
	 					$event["data"]["payload"]["processor1"] == "ok" && 
	 					$event["data"]["payload"]["processor2"] == "ok" &&
	 					count($event["data"]["headers"]) == 2
	 			) { 
	 				echo "OK \n";
	 			} else {
	 				echo "FAILED \n";
	 			}	 			
			}
		);		

		echo "Test Publish: \n";

		$susi->publish("test", array("TEST first run" => "start"), 
			// finish callback
			function($event) use($susi,$container,$p_id_1, $p_id_2, $c_id) {
				echo "Test RUN1 Finish Callback >>> ";

				//print_r($event);				
				if(
	 					$event["data"]["payload"]["processor1"] == "ok" && 
	 					$event["data"]["payload"]["processor2"] == "ok" && 
	 					count($event["data"]["headers"]) == 2
	 			) {
	 				echo "OK \n";
	 			} else {
	 				echo "FAILED \n";
	 			}

	 			// remove events and run secound time
	 			$susi->unregisterProcessor($p_id_1);
	 			$susi->unregisterConsumer($c_id);


	 			$susi->publish("test", array("TEST secound run" => "start"), 
	 				// finish callback
					function($event) use($susi,$container,$p_id_2) {

						echo "Test RUN2 Finish Callback >>> ";
						
						if(
			 					!isset($event["data"]["payload"]["processor1"]) && 
			 					$event["data"]["payload"]["processor2"] == "ok" && 
	 							count($event["data"]["headers"]) == 1
			 			) {
			 				echo "OK \n";
			 			} else {
			 				echo "FAILED \n";
			 			}


						echo "Cleanup:\n";
			 			$susi->unregisterProcessor($p_id_2);
			 			echo "TEST END:\n";
					}
				);	 			
			}
		);
	}
);

$susi->run();