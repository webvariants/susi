#include "susi/SusiClient.h"

namespace Susi {
	class HeartBeat {

	public:
		HeartBeat(Susi::SusiClient & susi);
		void join();

	protected:
		Susi::SusiClient & susi_;
		std::thread runloop_;
		std::atomic<bool> stopVar_{false};
	};
}
