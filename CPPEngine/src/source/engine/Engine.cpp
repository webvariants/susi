#include "engine/Engine.h"
#include "controller/SampleController.h"

void Susi::Cpp::Engine::populateController() {
	addController("sampleController", new Susi::Cpp::SampleController{});
}
