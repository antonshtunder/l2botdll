#ifndef ITEM_H
#define ITEM_H

#include "lineage/instance.h"
#include "utils.h"
#include "lineagerepresentation.h"


class Item : public Instance
{
public:
    Item(DWORD address);

    DWORD getID();
    DWORD getTypeID();
    DWORD getAmount();

    void makeRepresentation(ItemRepresentation &mobRep);

    static std::vector<Item> &getInventoryItems();

private:
    static std::vector<Item> inventoryItems;
};

#endif // ITEM_H
