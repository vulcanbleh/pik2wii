#ifndef _SYSTEM12_CONTROLLER_BATTERY_MGR_H
#define _SYSTEM12_CONTROLLER_BATTERY_MGR_H

#include <RevoSDK/wpad.h>
#include <egg/core/eggSingleton.h>

namespace System12 {

class ControllerBatteryMgr {
public:
	EGG_SINGLETON_DECL(ControllerBatteryMgr);
	
	ControllerBatteryMgr();
	
	void start_get_info(int);
	void calc();
	uint get_current_battery_state(int);
	void wpad_callback(u32, u32);
	
	//unused/inlined:
	
	void reset_current_info();
	void get_last_battery_state(int);
	
	// _00     = VTBL
	// _00-_10 = EGG::Disposer
	int mControllerNum; // _10
};


} // namespace System12

#endif
