#ifndef LINEAGEGLOBAL_H
#define LINEAGEGLOBAL_H

#include "player.h"
#include "mob.h"
#include "lineagerepresentation.h"
#include "droppeditem.h"

typedef void (__fastcall *FDoAction)(DWORD actionID, DWORD someAddr, DWORD zero1, DWORD zero2);


class LineageGlobal
{
public:
    LineageGlobal();

    static LineageGlobal *instance();
    static LPDWORD getPlayerTargetModelPointer();

    bool isInGame();
    bool isAddressInArray(DWORD address);

    Player getPlayer();
    std::vector<Mob> &getMobs();
    std::vector<DroppedItem> &getDroppedItems();

    void attack();
    void performActionOn(DWORD instanceID);
    void focusMob(DWORD address);

    LineageRepresentation getRepresentation();
private:
    DWORD getArraysAddress();
    DWORD getADDR1();

    void doAction(DWORD actionID);

    static LineageGlobal *_instance;
    static DWORD doActionOnInstanceFunction;
    static DWORD doActionOnInstanceECXArgument;

    DWORD _skillsInventoryBase;

    std::vector<Mob> _mobs;
    std::vector<DroppedItem> _droppedItems;

    FDoAction _doAction;
};

#endif // LINEAGEGLOBAL_H
