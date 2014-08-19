<?php

	error_reporting (E_ALL);

	define("_DIR", dirname(__FILE__));


	function __autoload($name){
		$paths = explode(PATH_SEPARATOR, get_include_path());
		array_push($paths, dirname(__FILE__));
		array_push($paths, dirname(__FILE__)."/ION");
		array_push($paths, dirname(__FILE__)."/vendor");
		set_include_path(implode(PATH_SEPARATOR, $paths));
		$name = implode("/",explode("_",$name)).".php";
		
		require $name;
	}

	$config = '/mnt/data/web/config.php';
			
	if (!file_exists($config)) {	
		$config = _DIR.'/config.php';
	}

	require $config;
	$susi = new Susi($CONFIG["SUSI_ADDR"], $CONFIG["SUSI_PORT"]);

	$container = new IONContainer($CONFIG);

	$susi->registerConsumer("php_controller",

		// callback
		function($event) use($susi,$container) {
			echo "PHP CONSUMER CALLBACK";

			$data = $event["data"];			

			if(isset($data["payload"])) {
				$data["payload"]["consumer"] = "ok";
			}			
		}
	);

	$susi->registerProcessor("php_controller",

		// callback
		function($event) use($susi,$container) {
			echo "PHP PROCESSOR CALLBACK";

			print_r($event);


			$data = $event["data"];			

			if(isset($data["payload"])) {

				if(isset($data["payload"]["controller"]) && isset($data["payload"]["action"])) {

					$className = "Controller".ucfirst($data["payload"]["controller"]);
					$actionName = $data["payload"]["action"]."Action";

					if(isset($event["payload"]) && isset($event["payload"]["device"])) {
						$className = "Device".ucfirst($event["payload"]["device"])."_".$className;
					}

					try{
						$controller = new $className($container);
						
						if(method_exists($controller,$actionName)){
							//$req = new Request($event);
							//$container->initForRequest($req);
							//$res = $controller->$actionName(new Request(array("env"=>array("body"=>$event))));							
							
							//$susi->publish($data["topic"], $res->getContent());							
							$susi->publish($data["topic"], array("Fake" => "ok"));							
						}else{
							$susi->publish($data["topic"], array("Error" => "no such action"));
						}
					}catch(Exception $e){
						$susi->publish($data["topic"], array("Error" => $e->getMessage()));
					}

				}
				$data["payload"]["processor"] = "ok";				
			} else {
				$data["payload"]["error"] = "controller or action missing!";
			}

			// processor has to acknolage event back to eventManager
			$susi->ack($data);
		}
	);

	$susi->run();