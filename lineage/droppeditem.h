#ifndef DROPPEDITEM_H
#define DROPPEDITEM_H

#include "instance.h"
#include "utils.h"
#include "lineagerepresentation.h"


class DroppedItem : public Instance
{
public:
    DroppedItem(DWORD address);

    DWORD getID();
    DWORD getTypeID();
    DWORD getAmount();
    DWORD getModel();
    Point3F *getLoc();

    static bool isValid(DWORD address);
    bool isValid();

    void makeRepresentation(DroppedItemRepresentation &mobRep);
};

#endif // DROPPEDITEM_H
