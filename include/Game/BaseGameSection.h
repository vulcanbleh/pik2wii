#ifndef _GAME_BASEGAMESECTION_H
#define _GAME_BASEGAMESECTION_H

#include "BuildSettings.h"
#include "Camera.h"
#include "DvdThreadCommand.h"
#include "Game/BaseHIO.h"
#include "Game/GameMessage.h"
#include "Game/PikiContainer.h"
#include "IDelegate.h"
#include "Rect.h"
#include "Splitter.h"
#include "Vector3.h"

struct BlackFader;
struct Controller;
struct WipeInFader;
struct WipeOutFader;
struct WipeOutInFader;

namespace PSGame {
struct SceneInfo;
} // namespace PSGame

namespace Sys {
struct DrawBuffers;
} // namespace Sys

namespace TreasureLight {
struct Mgr;
} // namespace TreasureLight

namespace Game {

namespace ItemCave {
struct Item;
} // namespace ItemCave

namespace ItemBigFountain {
struct Item;
} // namespace ItemBigFountain

namespace ItemHole {
struct Item;
} // namespace ItemHole

struct MovieConfig;
struct CameraArg;
struct Onyon;
struct Creature;
struct Piki;
struct CourseInfo;
struct GameMessage;
struct GameLightMgr;
struct Pellet;
struct PlayCamera;

enum DemoTimers {
	DEMOTIMER_None                   = 0,
	DEMOTIMER_Piki_Seed_In_Ground    = 1, // plays if you dont pluck the first red pikmin in 10 seconds
	DEMOTIMER_Camera_Tutorial        = 2, // Entirely unused, the camera tutorial was meant to use it, but nothing starts its timer
	DEMOTIMER_Unlock_Switch_To_Louie = 3,
	DEMOTIMER_Meet_Red_Pikmin        = 4, // 1 second timer after whistling the first red on day 1
	DEMOTIMER_Reds_Purples_Tutorial  = 5, // 10 seconds after you first have reds and purples in your party
	DEMOTIMER_Unk6                   = 6,
	DEMOTIMER_YouAppearLost          = 7, // When you wait 3 minutes on day 1 after growing 15 reds before crushing the first bag
};

enum DrawBufferType {
	DB_NormalLayer       = 0,
	DB_NormalFogOffLayer = 1,
	DB_MapLayer          = 2,
	DB_PikiLayer         = 3,
	DB_PostRenderLayer   = 4,
	DB_2DLayer           = 5,
	DB_FirstLayer        = 6,
	DB_PostShadowLayer   = 7,
	DB_ObjectLastLayer   = 8,
	DB_FarmLayer         = 9,
};

struct BaseGameSection : public BaseHIOSection {

	struct ZoomCamera : public LookAtCamera {
		virtual ~ZoomCamera() { } // _08 (weak)
		virtual void doUpdate();  // _78

		void init(f32 dist1, f32 dist2, Vector3f& lookAtPos, Controller* control);
		void makeLookAt();

		inline f32 getAngleX() const { return mAngleX; }

		// _00		= VTBL
		// _00-_198	= LookAtCamera
		f32 mAngleX;             // _198
		f32 mAngleY;             // _19C
		f32 mTargetDistance;     // _1A0
		f32 mUnusedDist;         // _1A4, set to same as above, not used for anything
		f32 mTargetFrontAngle;   // _1A8
		Controller* mController; // _1AC
	};

	BaseGameSection(struct JKRHeap* heap);

	/////////////////// VTABLE
	virtual ~BaseGameSection();                                                // _08
	virtual void init();                                                       // _18
	virtual void drawInit(Graphics& gfx, Section::EDrawInitMode drawInitMode); // _20
	virtual bool forceFinish()
	{
		mIsLoadingDVD = true;
		return mIsLoadingDVD;
	} // _28 (weak)
	virtual bool doUpdate();                                                  // _3C
	virtual void doDraw(Graphics& gfx);                                       // _40
	virtual void doEntry();                                                   // _44
	virtual bool sendMessage(GameMessage& msg) { return false; }              // _54 (weak)
	virtual void pre2dDraw(Graphics& gfx) { }                                 // _58 (weak)
	virtual int getCurrFloor() { return -1; }                                 // _5C (weak)
	virtual bool isDevelopSection() { return true; }                          // _60 (weak)
	virtual void addChallengeScore(int) { }                                   // _64 (weak)
	virtual void startMainBgm() { }                                           // _68 (weak)
	virtual void section_fadeout() { }                                        // _6C (weak)
	virtual void goNextFloor(ItemHole::Item* hole) { }                        // _70 (weak)
	virtual void goCave(ItemCave::Item* cave) { }                             // _74 (weak)
	virtual void goMainMap(ItemBigFountain::Item* geyser) { }                 // _78 (weak)
	virtual u32 getCaveID() { return 'none'; }                                // _7C (weak)
	virtual CourseInfo* getCurrentCourseInfo() { return nullptr; }            // _80 (weak)
	virtual bool challengeDisablePelplant() { return true; }                  // _84 (weak)
	virtual char* getCaveFilename() { return "caveinfo.txt"; }                // _88 (weak)
	virtual char* getEditorFilename() { return "random"; }                    // _8C (weak)
	virtual int getVsEditNumber() { return -2; }                              // _90 (weak)
	virtual bool openContainerWindow() { return false; }                      // _94 (weak)
	virtual void closeContainerWindow() { }                                   // _98 (weak)
	virtual void playMovie_firstexperience(int, Creature*) { }                // _9C (weak)
	virtual void playMovie_bootup(Onyon* onyon) { }                           // _A0 (weak)
	virtual void playMovie_helloPikmin(Piki* piki) { }                        // _A4 (weak)
	virtual void enableTimer(f32 value, u32 type) { }                         // _A8 (weak)
	virtual void disableTimer(u32 type) { }                                   // _AC (weak)
	virtual u32 getTimerType() { return DEMOTIMER_None; }                     // _B0 (weak)
	virtual void onMovieStart(MovieConfig* movie, u32 unused, u32 naviID) { } // _B4 (weak)
	virtual void onMovieDone(MovieConfig*, u32, u32) { }                      // _B8 (weak)
	virtual void onMovieCommand(int);                                         // _BC
	virtual void startFadeout(f32);                                           // _C0
	virtual void startFadein(f32);                                            // _C4
	virtual void startFadeoutin(f32);                                         // _C8
	virtual void startFadeblack();                                            // _CC
	virtual void startFadewhite();                                            // _D0
	virtual void gmOrimaDown(int) { }                                         // _D4 (weak)
	virtual void gmPikminZero() { }                                           // _D8 (weak)
	virtual void openCaveInMenu(ItemCave::Item*, int) { }                     // _DC (weak)
	virtual void openCaveMoreMenu(ItemHole::Item*, Controller*) { }           // _E0 (weak)
	virtual void openKanketuMenu(ItemBigFountain::Item*, Controller*) { }     // _E4 (weak)
	virtual void on_setCamController(int) { }                                 // _E8 (weak)
	virtual void onTogglePlayer() { }                                         // _EC (weak)
	virtual void onPlayerJoin() { }                                           // _F0 (weak)
	virtual void onInit() { }                                                 // _F4 (weak)
	virtual void onUpdate() { }                                               // _F8 (weak)
	virtual void initJ3D();                                                   // _FC
	virtual void initViewports(Graphics& gfx);                                // _100
	virtual void initResources();                                             // _104
	virtual void initGenerators();                                            // _108
	virtual void initLights();                                                // _10C
	virtual void draw3D(Graphics& gfx);                                       // _110
	virtual void draw2D(Graphics& gfx);                                       // _114
	virtual void drawParticle(Graphics& gfx, int viewportIndex);              // _118
	virtual void draw_Ogawa2D(Graphics& gfx);                                 // _11C
	virtual void do_drawOtakaraWindow(Graphics& gfx);                         // _120
	virtual void onSetupFloatMemory() { }                                     // _124 (weak)
	virtual void postSetupFloatMemory();                                      // _128
	virtual void onSetSoundScene() { }                                        // _12C (weak)
	virtual void onStartHeap() { }                                            // _130 (weak)
	virtual void onClearHeap() { }                                            // _134 (weak)
	/////////////////// VTABLE END

	void useSpecificFBTexture(JUTTexture*);
	void restoreFBTexture();
	void loadSync(IDelegate*, bool);
	u32 waitSyncLoad(bool);
	void dvdloadGameSystem();
	void movieDone(Game::MovieConfig*, u32, u32);
	void advanceDayCount();
	void saveToGeneratorCache(Game::CourseInfo*);
	void pmTogglePlayer();
	void pmPlayerJoin();
	void setPlayerMode(int);
	void onCameraBlendFinished(Game::CameraArg*);
	void setFixNearFar(bool, f32, f32);
	void setCamController();
	void setDefaultPSSceneInfo(PSGame::SceneInfo&);
	void prepareHoleIn(Vector3f&, bool);
	void prepareFountainOn(Vector3f&);
	void test_draw_treasure_detector();
	void directDraw(Graphics& gfx, Viewport*);
	void directDrawPost(Graphics& gfx, Viewport*);
	void j3dSetView(Viewport*, bool);
	void doSimulation(f32);
	void doSimpleDraw(Viewport*);
	void doAnimation();
	void changeGeneratorCursor(Vector3f&);
	void doSetView(int viewportNumber);
	void doViewCalc();
	void updateBlendCamera();
	void setSplitter(bool);
	void updateSplitter();
	void doDirectDrawPost(Graphics& gfx, Viewport*);
	void doDirectDraw(Graphics& gfx, Viewport*);
	void clearHeap();
	void setupFixMemory();
	void setupFixMemory_dvdload();
	bool enableAllocHalt();
	bool disableAllocHalt();
	void setupFloatMemory();
	void setDrawBuffer(int);
	void createFallPikminSound();
	void captureRadarmap(Graphics& gfx);

	inline void newdraw_draw3D(Graphics& gfx);
	void newdraw_draw3D_all(Graphics& gfx);
	void newdraw_drawAll(Viewport*);

	void drawOtakaraWindow(Graphics& gfx);
	void setDraw2DCreature(Game::Creature*);
	void startZoomWindow();
	void startKantei2D();
	void onKanteiDone(Rect<f32>&);

	int getNumWindows();
	int getActivePlayerID();
	void setupViewportMatrix(Graphics&);
	void j3dDraw(Viewport*);
	void j3dDrawPostShadow(Viewport*);
	void j3dDrawObjectLast(Viewport*);
	void j3dDrawPost(Viewport*);
	void j3dDrawLast(Viewport*);
	void j3dViewCalc(Viewport*);

	void initBlendCamera();
	void blend1to2();
	void blend2to1();

	void startSplit();
	void changeSplit();
	void endSplit();

	void startHeap();
	bool isAllocHalt();

	void drawRadarmap(Graphics&);

	inline void setXfbBounds(int x, int y)
	{
		mXfbBoundsX = x;
		mXfbBoundsY = y;
	}

	static u8 sOptDraw;

	// _00 		= VTBL
	// _00-_4C 	= BaseHIOSection
	// All the remaining unnamed variables seem to be unused.
	u32 mMoney;                                               // _4C
	u32 _50;                                                  // _50
	BlendCamera* mBlendCamera;                                // _54
	f32 mBlendFactor;                                         // _58
	bool mIsBlendCameraActive;                                // _5C
	DvdThreadCommand mDvdThreadCommand;                       // _60
	IDelegate3<MovieConfig*, u32, u32>* mMovieFinishCallback; // _CC
	IDelegate3<MovieConfig*, u32, u32>* mMovieStartCallback;  // _D0
	BlackFader* mBlackFader;                                  // _D4
	WipeInFader* mWipeInFader;                                // _D8
	WipeOutFader* mWipeOutFader;                              // _DC
	WipeOutInFader* mWipeOutInFader;                          // _E0
	u32 mUnusedVal;                                           // _E4
	int mPrevNaviIdx;                                         // _E8
	f32 mSecondViewportHeight;                                // _EC
	f32 mSplit;                                               // _F0
	u8 mSetSplit;                                             // _F4
	JUTTexture* mMizuTexture;                                 // _F8
	TreasureLight::Mgr* mTreasureLightMgr;                    // _FC
	JKRExpHeap* mTheExpHeap;                                  // _100
	JKRHeap* mBackupHeap;                                     // _104
	PlayCamera* mOlimarCamera;                                // _108
	PlayCamera* mLouieCamera;                                 // _10C
	Controller* mControllerP1;                                // _110
	Controller* mControllerP2;                                // _114
	int mPlayerMode;                                          // _118
	Splitter* mSplitter;                                      // _11C
	u8 mUnusedFlag;                                           // _120, true by default, set false in single and vs game
	int _124;                                                 // _124
	u32 _128;                                                 // _128
	GameLightMgr* mLightMgr;                                  // _12C
	Sys::DrawBuffers* mOpaqueDrawBuffer;                      // _130
	Sys::DrawBuffers* mTransparentDrawBuffer;                 // _134
	int mTreasureGetState;                                    // _138
	Viewport* mTreasureGetViewport;                           // _13C
	Creature* mDraw2DCreature;                                // _140
	f32 mDraw2DCreatureScale;                                 // _144
	f32 mUnused2DCreatureVal;                                 // _148
	Delegate1<BaseGameSection, Rectf&>* mKanteiDelegate;      // _14C
	ZoomCamera* mTreasureZoomCamera;                          // _150
	u32 _154;                                                 // _154
	JUTTexture* mXfbImage;                                    // _158
	JUTTexture* mFbTexture;                                   // _15C
	int mXfbBoundsX;                                          // _160
	int mXfbBoundsY;                                          // _164
	u8 mXfbFlags;                                             // _168
	JUTTexture* mXfbTexture2d;                                // _16C
	int mXfbBounds2dX;                                        // _170
	int mXfbBounds2dY;                                        // _174
};
} // namespace Game

#endif
