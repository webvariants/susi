#ifndef __PLUGIN__
#define __PLUGIN__

namespace Susi {

class Plugin{
public:
	virtual ~Plugin(){}
	virtual void run() = 0;
};

}

#endif // __PLUGIN__