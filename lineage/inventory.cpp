#include "inventory.h"
#include "lineageglobal.h"

Inventory::Inventory(DWORD address):
    Instance(address)
{}

Inventory Inventory::getNormalInventory()
{
    return *reinterpret_cast<LPDWORD>(getInventoryWindow() + 0xB0);
}

Inventory Inventory::getQuestInventory()
{
    return *reinterpret_cast<LPDWORD>(getInventoryWindow() + 0xB4);
}

DWORD Inventory::getInventoryWindow()
{
    auto base = LineageGlobal::instance()->getSibBase();
    return *reinterpret_cast<LPDWORD>(base + 0x440);
}
