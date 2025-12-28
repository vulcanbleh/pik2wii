#ifndef _GAME_SINGLEGAMESECTION_H
#define _GAME_SINGLEGAMESECTION_H

#include "DvdThreadCommand.h"
#include "Game/BaseGameSection.h"
#include "Game/gamePlayData.h"
#include "Game/gameStages.h"
#include "efx/TChasePos.h"
#include "Radar.h"
#include "id32.h"

namespace og {
namespace Screen {
struct DispMemberSMenuAll;
}
} // namespace og

namespace Game {
namespace SingleGame {
struct FSM;
struct GameState;
struct State;
} // namespace SingleGame

/**
 * @Size{0x278}
 */
struct SingleGameSection : public BaseGameSection {
	typedef Game::SingleGame::State StateType;

	SingleGameSection(JKRHeap*);

	virtual ~SingleGameSection();                                             // _08
	virtual bool doUpdate();                                                  // _3C
	virtual void doDraw(Graphics& gfx);                                       // _40
	virtual int getCurrFloor() { return mCurrentFloor; }                      // _58 (weak)
	virtual bool isDevelopSection() { return false; }                         // _5C (weak)
	virtual void startMainBgm();                                              // _64
	virtual void section_fadeout();                                           // _68
	virtual void goNextFloor(ItemHole::Item* hole);                           // _6C
	virtual void goCave(ItemCave::Item* cave);                                // _70
	virtual void goMainMap(ItemBigFountain::Item* geyser);                    // _74
	virtual u32 getCaveID() { return mCaveID.getID(); }                       // _78 (weak)
	virtual CourseInfo* getCurrentCourseInfo() { return mCurrentCourseInfo; } // _7C (weak)
	virtual void playMovie_firstexperience(int, Creature*);                   // _98
	virtual void playMovie_bootup(Onyon* onyon);                              // _9C
	virtual void playMovie_helloPikmin(Piki* piki);                           // _A0
	virtual void enableTimer(f32 value, u32 type);                            // _A4
	virtual void disableTimer(u32 type);                                      // _A8
	virtual void onMovieStart(MovieConfig* movie, u32 unused, u32 naviID);    // _B0
	virtual void onMovieDone(MovieConfig*, u32, u32);                         // _B4
	virtual void onMovieCommand(int);                                         // _B8
	virtual void gmOrimaDown(int);                                            // _D0
	virtual void gmPikminZero();                                              // _D4
	virtual void openCaveInMenu(ItemCave::Item*, int);                        // _D8
	virtual void openCaveMoreMenu(ItemHole::Item*, Controller*);              // _DC
	virtual void openKanketuMenu(ItemBigFountain::Item*, Controller*);        // _E0
	virtual void on_setCamController(int);                                    // _E4
	virtual void onInit();                                                    // _F0
	virtual void onSetupFloatMemory();                                        // _120
	virtual void onSetSoundScene();                                           // _128
	virtual void onStartHeap();                                               // _12C
	virtual void onClearHeap();                                               // _130
	virtual u32 getTimerType() { return mTimerType; }                         // _AC (weak)
	virtual char* getCaveFilename() { return mCaveFilename; }                 // _84 (weak)

	void clearCaveMenus();
	void clearCaveOtakaraEarningsAndDrops();
	void createFallPikmins();
	void flow_goto_title();
	void loadMainMapSituation();
	void newCaveOtakaraEarningsAndDrops();
	void saveCaveMore();
	void saveMainMapSituation(bool isSubmergedCastle);
	void setDispMemberSMenu(og::Screen::DispMemberSMenuAll&);
	void setupCaveGames();
	void setupMainMapGames();
	bool updateCaveMenus();
	void updateCaveScreen();
	void updateMainMapScreen();
	void drawCaveScreen();
	void drawMainMapScreen();
	void setDispMemberNavi(og::Screen::DataNavi& data, int naviID);
	Radar::Mgr::RadarSearchResult calcOtakaraLevel(f32&);

	inline void setCurrState(StateType* state) { mCurrentState = state; }
	inline StateType* getCurrState() { return mCurrentState; }

	f32 mTimer;                                           // _178
	bool mTimerEnabled;                                   // _17C
	u32 mTimerType;                                       // _180
	u8 mOpenMenuFlags;                                    // _184
	struct Game::ItemCave::Item* mCurrentCave;            // _188
	struct Game::ItemHole::Item* mHole;                   // _18C
	struct Game::ItemBigFountain::Item* mFountain;        // _190
	u32 mCaveIndex;                                       // _194
	bool mIsExitingMap;                                   // _198
	char mCaveFilename[0x20];                             // _199
	bool mDoEnd;                                          // _1B9
	DvdThreadCommand mDvdThread;                          // _1BC
	Delegate<Game::SingleGameSection>* mLoadGameCallback; // _228
	u8 mIsGameStarted;                                    // _22C, used to tell if new entry effects should play on opening the world map
	Game::CourseInfo* mCurrentCourseInfo;                 // _230
	ID32 mCaveID;                                         // _234
	bool mInCave;                                         // _240
	bool mTreasureRadarActive;                            // _241
	bool mNeedTreasureCalc;                               // _242
	Delegate<Game::SingleGameSection>* mCaveSaveCallback; // _244
	efx::TChasePos* mWeatherEfx;                          // _248
	int mCurrentFloor;                                    // _24C
	Game::SingleGame::FSM* mFsm;                          // _250
	Game::SingleGame::State* mCurrentState;               // _254
	KindCounter mCaveTreasureCounter;                     // _258
	KindCounter mCaveUpgradeCounter;                      // _260
	KindCounter mOtakaraCounter;                          // _268
	KindCounter mItemCounter;                             // _270
	u8 mDoTrackCarcass;                                   // _278
};
} // namespace Game

#endif
