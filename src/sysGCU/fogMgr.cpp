#include "FogMgr.h"
#include "Camera.h"
#include "Graphics.h"
#include "RevoSDK/gx.h"
#include "RevoSDK/vi.h"
#include "System.h"
#include "Viewport.h"
#include "nans.h"
#include "types.h"

/**
 * __ct
 *
 * @note Address: 0x80432948
 * @note Size: 0x74
 */
FogMgr::FogMgr()
    : CNode("フォグマネージャ")
    , mType(GX_FOG_LINEAR)
    , mNearZ(640.0f)
    , mFarZ(3024.0f)
    , mColor(173, 177, 252, 255)
{
}

/**
 * @note Address: 0x804329BC
 * @note Size: 0x40
 */
void FogMgr::off(Graphics&)
{
	GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, mColor.GXColorView);
}

/**
 * @note Address: 0x804329FC
 * @note Size: 0xAC
 */
void FogMgr::set(Graphics& graphics)
{
	GXFogAdjTable table;
	Camera* activeCam = graphics.mCurrentViewport->mCamera;

	GXSetFog(mType, mNearZ, mFarZ, activeCam->getNear(), activeCam->getFar(), mColor.GXColorView);

	u16 width = System::getRenderModeObj()->fbWidth;
	GXInitFogAdjTable(&table, width, activeCam->mProjectionMtx);

	width = System::getRenderModeObj()->fbWidth;
	GXSetFogRangeAdj(GX_TRUE, width / 2, &table);
}

/**
 * @note Address: 0x80432AA8
 * @note Size: 0x24
 */
void FogMgr::setColor(Color4& color)
{
	mColor.GXColorView.r = color.r;
	mColor.GXColorView.g = color.g;
	mColor.GXColorView.b = color.b;
	mColor.GXColorView.a = color.a;
}

/**
 * @note Address: 0x80432ACC
 * @note Size: 0x24
 */
void FogMgr::getColor(Color4& color)
{
	color.r = mColor.GXColorView.r;
	color.g = mColor.GXColorView.g;
	color.b = mColor.GXColorView.b;
	color.a = mColor.GXColorView.a;
}
