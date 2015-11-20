#include "susi/SusiClient.h"

namespace Susi {
	class Webhooks {
	public:
		Webhooks(Susi::SusiClient & susi);
		void join();

	protected:
		Susi::SusiClient & susi_;
		boost::asio::io_service io_service_;
	};
}
