#include "Game/Entities/Item.h"
#include "System.h"
#include "nans.h"

// TODO: fix this up
static void __Print(const char** fmt, ...)
{
	*fmt = "registItem";
}

namespace Game {


/**
 * @note Address: 0x801E4B98
 * @note Size: 0x664
 */
void ItemMgr::createManagers(u32 flags)
{
	bool isGround = (flags >> 1) & 1;
	bool isCave   = flags & 1;

	sys->heapStatusStart("-Barrel-", nullptr);
	ItemBarrel::mgr = new ItemBarrel::Mgr();
	ItemBarrel::mgr->loadResources();
	itemMgr->addMgr(ItemBarrel::mgr);
	sys->heapStatusEnd("-Barrel-");

	sys->heapStatusStart("-Ujamushi-", nullptr);
	ItemUjamushi::mgr = new ItemUjamushi::Mgr();
	ItemUjamushi::mgr->loadResources();
	itemMgr->addMgr(ItemUjamushi::mgr);
	sys->heapStatusEnd("-Ujamushi-");

	sys->heapStatusStart("-Weed-", nullptr);
	if (isGround) {
		ItemWeed::mgr = new ItemWeed::Mgr;
		ItemWeed::mgr->loadResources();
		itemMgr->addMgr(ItemWeed::mgr);
	} else {
		ItemWeed::mgr = nullptr;
	}
	sys->heapStatusEnd("-Weed-");

	sys->heapStatusStart("-DownFloor-", nullptr);
	ItemDownFloor::mgr = new ItemDownFloor::Mgr();
	ItemDownFloor::mgr->loadResources();
	itemMgr->addMgr(ItemDownFloor::mgr);
	sys->heapStatusEnd("-DownFloor-");

	sys->heapStatusStart("-Treasure-", nullptr);
	ItemTreasure::mgr = new ItemTreasure::Mgr();
	ItemTreasure::mgr->loadResources();
	itemMgr->addMgr(ItemTreasure::mgr);
	sys->heapStatusEnd("-Treasure-");

	sys->heapStatusStart("-PikiHead-", nullptr);
	ItemPikihead::mgr = new ItemPikihead::Mgr();
	ItemPikihead::mgr->loadResources();
	itemMgr->addMgr(ItemPikihead::mgr);
	sys->heapStatusEnd("-PikiHead-");

	sys->heapStatusStart("-Plant-", nullptr);
	if (isGround) {
		ItemPlant::mgr = new ItemPlant::Mgr;
		ItemPlant::mgr->loadResources();
		itemMgr->addMgr(ItemPlant::mgr);
	} else {
		ItemPlant::mgr = nullptr;
	}
	sys->heapStatusEnd("-Plant-");

	sys->heapStatusStart("-Rock-", nullptr);
	if (isGround) {
		ItemRock::mgr = new ItemRock::Mgr;
		ItemRock::mgr->loadResources();
		itemMgr->addMgr(ItemRock::mgr);
	} else {
		ItemRock::mgr = nullptr;
	}
	sys->heapStatusEnd("-Rock-");

	sys->heapStatusStart("-Honey-", nullptr);
	ItemHoney::mgr = new ItemHoney::Mgr();
	ItemHoney::mgr->loadResources();
	itemMgr->addMgr(ItemHoney::mgr);
	sys->heapStatusEnd("-Honey-");

	// Had to be custom
	sys->heapStatusStart("-Onyon-", nullptr);
	ItemOnyon::mgr = new ItemOnyon::Mgr;
	itemMgr->addMgr(ItemOnyon::mgr);
	ItemOnyon::mgr->init();
	sys->heapStatusEnd("-Onyon-");

	sys->heapStatusStart("-Hole-", nullptr);
	if (isCave) {
		ItemHole::mgr = new ItemHole::Mgr;
		ItemHole::mgr->loadResources();
		itemMgr->addMgr(ItemHole::mgr);
	} else {
		ItemHole::mgr = nullptr;
	}
	sys->heapStatusEnd("-Hole-");

	sys->heapStatusStart("-Cave-", nullptr);
	if (isGround) {
		ItemCave::mgr = new ItemCave::Mgr;
		ItemCave::mgr->loadResources();
		itemMgr->addMgr(ItemCave::mgr);
	} else {
		ItemCave::mgr = nullptr;
	}
	sys->heapStatusEnd("-Cave-");

	sys->heapStatusStart("-BigFountain-", nullptr);
	if (isCave) {
		ItemBigFountain::mgr = new ItemBigFountain::Mgr;
		ItemBigFountain::mgr->loadResources();
		itemMgr->addMgr(ItemBigFountain::mgr);
	} else {
		ItemBigFountain::mgr = nullptr;
	}
	sys->heapStatusEnd("-BigFountain-");

	sys->heapStatusStart("-Bridge-", nullptr);
	if (isGround) {
		ItemBridge::mgr = new ItemBridge::Mgr;
		ItemBridge::mgr->loadResources();
		itemMgr->addMgr(ItemBridge::mgr);
	} else {
		ItemBridge::mgr = nullptr;
	}
	sys->heapStatusEnd("-Bridge-");

	sys->heapStatusStart("-Gate-", nullptr);
	itemGateMgr = new ItemGateMgr();
	itemMgr->addMgr(itemGateMgr);
	sys->heapStatusEnd("-Gate-");

	sys->heapStatusStart("-DengekiGate-", nullptr);
	if (isGround) {
		ItemDengekiGate::mgr = new ItemDengekiGate::Mgr;
		itemMgr->addMgr(ItemDengekiGate::mgr);
	} else {
		ItemDengekiGate::mgr = nullptr;
	}
	sys->heapStatusEnd("-DengekiGate-");
}

/**
 * @note Address: 0x801E51FC
 * @note Size: 0x48
 */
void ItemMgr::clearGlobalPointers()
{
	ItemBarrel::mgr      = nullptr;
	ItemUjamushi::mgr    = nullptr;
	ItemWeed::mgr        = nullptr;
	ItemDownFloor::mgr   = nullptr;
	ItemTreasure::mgr    = nullptr;
	ItemPikihead::mgr    = nullptr;
	ItemPlant::mgr       = nullptr;
	ItemRock::mgr        = nullptr;
	ItemHoney::mgr       = nullptr;
	ItemOnyon::mgr       = nullptr;
	ItemHole::mgr        = nullptr;
	ItemCave::mgr        = nullptr;
	ItemBigFountain::mgr = nullptr;
	ItemBridge::mgr      = nullptr;
	itemGateMgr          = nullptr;
	ItemDengekiGate::mgr = nullptr;
}

/**
 * @note Address: 0x801E5244
 * @note Size: 0xE8
 */
void ItemMgr::killAllExceptOnyonMgr()
{
	if (ItemPlant::mgr) {
		ItemPlant::mgr->killAll();
		if (gameSystem) {
			gameSystem->detachObjectMgr(ItemPlant::mgr);
			itemMgr->delNode(ItemPlant::mgr);
		}
		ItemPlant::mgr = nullptr;
	}

	if (ItemRock::mgr) {
		ItemRock::mgr->killAll();
		if (gameSystem) {
			gameSystem->detachObjectMgr(ItemRock::mgr);
			itemMgr->delNode(ItemRock::mgr);
		}
		ItemRock::mgr = nullptr;
	}

	if (ItemHoney::mgr) {
		ItemHoney::mgr->killAll();
		if (gameSystem) {
			gameSystem->detachObjectMgr(ItemHoney::mgr);
			itemMgr->delNode(ItemHoney::mgr);
		}
		ItemHoney::mgr = nullptr;
	}
}

} // namespace Game
