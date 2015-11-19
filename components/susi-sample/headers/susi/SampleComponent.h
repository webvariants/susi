#include "susi/SusiClient.h"

namespace Susi {

    class SampleComponent {

    public:
        SampleComponent(Susi::SusiClient & susi, const BSON::Value & config);
        ~SampleComponent();
        void join();

    protected:
        SusiClient & _susi;
        const BSON::Value & _config;
    };

}
