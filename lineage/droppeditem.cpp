#include "droppeditem.h"

DroppedItem::DroppedItem(DWORD address):
    Instance(address)
{

}

DWORD DroppedItem::getID()
{
    return *reinterpret_cast<LPDWORD>(_address);
}

DWORD DroppedItem::getTypeID()
{
    return *reinterpret_cast<LPDWORD>(_address + 4);
}

DWORD DroppedItem::getAmount()
{
    return *reinterpret_cast<LPDWORD>(_address + 0xC);
}

DWORD DroppedItem::getModel()
{
    return *reinterpret_cast<LPDWORD>(_address + 0x14);
}

Point3F *DroppedItem::getLoc()
{
    return reinterpret_cast<Point3F*>(getModel() + 0x1B4);
}

bool DroppedItem::isValid(DWORD address)
{
    DroppedItem item(address);
    return (item.getID() > 0x10000000 && item.getID() < 0x20000000 &&
            item.getModel() > 0x10000 &&
            (*reinterpret_cast<LPDWORD>(address + 0x18) == 0) &&
            (*reinterpret_cast<LPDWORD>(address + 0x10) == 0))
            ? true : false;
}

bool DroppedItem::isValid()
{
    return DroppedItem::isValid(_address);
}

void DroppedItem::makeRepresentation(DroppedItemRepresentation &itemRep)
{
    itemRep.id = getID();
    itemRep.typeID = getTypeID();
    itemRep.x = getLoc()->x;
    itemRep.y = getLoc()->y;
    itemRep.z = getLoc()->z;
    itemRep.address = _address;
    itemRep.amount = getAmount();
}
