#ifndef __MAF_CLIENT_H__
#define __MAF_CLIENT_H__

#include "application.h"

class Client : public Application {
public:
	Client();
	~Client();
	virtual void initScene();
};

#endif
