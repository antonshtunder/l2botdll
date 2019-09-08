#include "item.h"
#include "lineage/lineageglobal.h"

std::vector<Item> Item::inventoryItems;

Item::Item(DWORD address):
    Instance(address)
{

}

DWORD Item::getID()
{
    return *reinterpret_cast<LPDWORD>(_address + 0xF8);
}

DWORD Item::getTypeID()
{
    return *reinterpret_cast<LPDWORD>(_address + 0xFC);
}

DWORD Item::getAmount()
{
    return *reinterpret_cast<LPDWORD>(_address + 0x104);
}

void Item::makeRepresentation(ItemRepresentation &itemRep)
{
    itemRep.id = getID();
    itemRep.typeID = getTypeID();
    itemRep.address = _address;
    itemRep.amount = getAmount();
}

std::vector<Item> &Item::getInventoryItems()
{
    inventoryItems.clear();
    auto base = LineageGlobal::instance()->getSibBase();
    auto inventoryBase = *reinterpret_cast<LPDWORD>(*reinterpret_cast<LPDWORD>(
                            *reinterpret_cast<LPDWORD>(base + 0x440) + 0xB0) + 0x88);
    auto inventoryArray = *reinterpret_cast<LPDWORD*>(inventoryBase + 0x28C);
    auto arraySize = *reinterpret_cast<LPDWORD>(inventoryBase + 0x290);
    for(DWORD i = 0; i < arraySize; ++i)
    {
        inventoryItems.push_back(Item(inventoryArray[i]));
    }
    return inventoryItems;
}
