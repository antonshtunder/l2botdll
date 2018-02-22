#ifndef INVENTORY_H
#define INVENTORY_H

#include "instance.h"

class Inventory : public Instance
{
public:
    Inventory(DWORD address);

    static Inventory getNormalInventory();
    static Inventory getQuestInventory();

private:
    static DWORD getInventoryWindow();
};

#endif // INVENTORY_H
