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

			$data = $event["data"];			

			if(isset($data["payload"])) {
				$data["payload"]["processor"] = "ok";				
			}			

			// processor has to acknolage event back to eventManager
			$susi->ack($data);
		}
	);

	$susi->run();