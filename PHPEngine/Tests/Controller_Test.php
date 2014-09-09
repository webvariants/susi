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
 // Event structur	
 //  [data] => Array
 //        (
 //            [headers] => Array()        // used for dynamic 
 //            [id] => xxxxxxxxxxxxxxxxxx  // unique id
 //            [payload] =>                // payload 
 //            [sessionid] =>              // sessionID
 //            [topic] => heartbeat::five  // Eventname
 //        )
 //    [type] => processorEvent            // Eventtype 


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
$p1 = $susi->registerProcessor("heartbeat::five",

		// handler
		function(&$event) use($susi) {

			// add some header infos
			$event["data"]["headers"][] = array("foo" => "bar");

			echo "processor 1 called \n";
			//print_r($event);
		}
	);

$p2 = $susi->registerProcessor("heartbeat::five",

		// handler
		function(&$event) use($susi) {
			echo "processor 2 called \n";
			//print_r($event);
		}
	);

$p3 = $susi->registerProcessor("Test_finished",

		// handler
		function(&$event) use($susi) {
			echo "processor 3 called \n";
			print_r($event);
			
			// stop test
			exit(0);
		}
	);

/*
 *  Register Consumer
 * 
 * @param string           Eventname
 * @param function($event) Handler, for consumer logic
 *
 * @return interger        unique EventID, can be used for unregister 
 */
$c1 = $susi->registerProcessor("heartbeat::five",

		// handler
		function(&$event) use($susi) {
			echo "test_consumer called \n";
			
			// Publish Event

			$evt = new Event("Test_finished", 
				array("TEST secound run" => "start"),
				array(array("foo" => "started"))
			);

			$susi->publish($evt, 
 				// finish callback
				function($event) use($susi) {

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
				}
			);	 
		}
	);


/*
 * start Susi
 */

$susi->run();