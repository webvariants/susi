<?php


class ControllerFoo {

	protected $container;

	public function __construct(IONContainer $container) {
		$this->container = $container;
	}


	public function barAction(Request $request, $ctx=null, $checksum=null) {		
		
		$response = array(
			"ok" => "doki"
		);

		return $response;
	}
}	