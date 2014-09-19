#include "engine/Engine.h"

void Susi::Cpp::Engine::populateController() {
	addController("sampleController", new Susi::Cpp::SampleController());
}
