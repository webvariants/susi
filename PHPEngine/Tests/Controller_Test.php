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

 // INFO 
 // pass $event in processor and consumer handler as reference !!!
 

define("_DIR", dirname(__FILE__));

require _DIR.'/../Susi.php';
require _DIR.'/../config.php';

/*
 * create Susi Instance
 */
$susi = new Susi($CONFIG["SUSI_ADDR"], $CONFIG["SUSI_PORT"], true);


/*
 *  Register Processors
 * 
 * @param string           Eventname
 * @param function($event) Handler, for processor logic
 *
 * @return interger        unique EventID, can be used for unregister 
 */
$p1 = $susi->registerProcessor("test_event",

		// handler
		function(&$event) use($susi) {

			echo "processor 1 handler called ";

			$data = $event->getData();

			// test event data from publish
			if(
 				isset($data["payload"]["number"]) && 
 				$data["payload"]["number"] == 1 && 
				count($data["headers"]) == 1
 			) {
 				echo "OK \n";
 			} else {
 				echo "FAILED \n";
 			}

			// add some header infos
			$event->setHeader(array("added Header" => "by processor 1"));

			// get payload
			$payload = $event->getPayload();
			// add payload
			$payload["add_one"] = $payload["number"] + 1;
			$payload["test_payload"] = "ok1";
			$event->setPayload($payload);

			
		},
		4
	);

$p2 = $susi->registerProcessor("test_event",

		// handler
		function(&$event) use($susi) {
			echo "processor 2 handler called ";

			$data = $event->getData();

			// test event data from processor 1
			if(
 				isset($data["payload"]["test_payload"]) && 
 				$data["payload"]["test_payload"] == "ok1" && 
				count($data["headers"]) == 2
 			) {
 				echo "OK \n";
 			} else {
 				echo "FAILED \n";
 			}


			// add some header infos
			$event->setHeader(array("added Header" => "by processor 2"));

			// change payload
			$payload = $event->getPayload();
			$payload["mult_five"] = $payload["add_one"] * 5;
			$payload["test_payload"] .= "_ok2";
			$event->setPayload($payload);
		},
		4
	);

/*
 *  Register Consumer
 * 
 * @param string           Eventname
 * @param function($event) Handler, for consumer logic
 *
 * @return interger        unique EventID, can be used for unregister 
 */
$c1 = $susi->registerConsumer("test_event",

		// handler
		function(&$event) use($susi) {
			echo "consumer handler called ";

			$data = $event->getData();
			
			// test event data
			if(
 				isset($data["payload"]["test_payload"]) && 
 				$data["payload"]["test_payload"] == "ok1_ok2" && 
				count($data["headers"]) == 3
 			) {
 				echo "OK \n";
 			} else {
 				echo "FAILED \n";
 			}
		},
		4
	);


$GLOBALS['test_starter_id'] = $susi->registerProcessor("heartbeat::one",

		// handler
		function(&$event) use($susi, $p1, $p2, $c1) {
			echo "Start Test \n";

			// Publish Event
			$evt = new Event("test_event", 
				array("number" => 1),
				array(array("header_info" => "en_en")),
				4
			);

			$susi->publish($evt, 
 				// finish callback
				function($event) use($susi, $p1, $p2, $c1) {
					echo "<<< Test Finished Callback called >>> \n";	
					//print_r($event->getData());									

					// remove events
	 				$susi->unregisterProcessor($p1);
	 				$susi->unregisterProcessor($p2);
	 				$susi->unregisterConsumer($c1);

	 				// use this
	 				$susi->unregisterProcessor($GLOBALS['test_starter_id']);	 				
	 				// or stop test with 
	 				exit(0);
				}
			);	 
					
		}
	);


/*
 * start Susi
 */

$susi->run();