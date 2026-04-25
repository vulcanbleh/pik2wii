#ifndef _CONTROLLER_H
#define _CONTROLLER_H

#include "egg/core/eggController.h"

struct Controller {
	Controller(int);

	int mContNum;
	EGG::CoreController* mCont;
};

#endif
