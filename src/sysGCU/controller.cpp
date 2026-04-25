#include "Controller.h"

Controller::Controller(int port)
{
	mContNum = port;
	mCont = EGG_INSTANCE(EGG::CoreControllerMgr)->getNthController(port);
}