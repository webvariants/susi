#include "susi/SusiClient.h"

namespace Susi {
	class StateFileComponent {

	public:
		StateFileComponent(Susi::SusiClient & susi, BSON::Value & config);
		void join();

	protected:
		Susi::SusiClient & susi_;
		std::string stateFile_;
		BSON::Value state_;

		bool validateFieldsForPut(const Susi::EventPtr & event);
		bool validateFieldsForGet(const Susi::EventPtr & event);

		void handlePut(Susi::EventPtr event);
		void handleGet(Susi::EventPtr event);
		void handleDelete(Susi::EventPtr event);

		bool readStateFile();
		bool writeStateFile();

	};
}
