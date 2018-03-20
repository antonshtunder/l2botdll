#ifndef LINEAGEGLOBAL_H
#define LINEAGEGLOBAL_H

#include "player.h"
#include "mob.h"
#include "lineagerepresentation.h"
#include "droppeditem.h"
#include "skill.h"
#include "lineage/l2utils/macro.h"

typedef void (__fastcall *FDoAction)(DWORD actionID, DWORD someAddr, DWORD zero1, DWORD zero2);
typedef void (__stdcall *FUseSkill)(DWORD macroAddr, DWORD zero);


class LineageGlobal
{
    friend void moveFuncTrampoline();
public:
    LineageGlobal();

    static LineageGlobal *instance();
    static LPDWORD getPlayerTargetModelPointer();

    bool isInGame();
    bool isDroppedItemPresent(DWORD address);

    Player getPlayer();
    std::vector<Mob> &getMobs();
    std::vector<DroppedItem> &getDroppedItems();

    void doAction(DWORD actionID);
    void attack();
    void assist();
    void performActionOn(DWORD instanceID);
    void focusMob(DWORD address);
    void useSkill(DWORD skillID);
    void moveTo(float x, float y, float z);
    void npcChat(DWORD index);
    void acceptAction();

    DWORD getNpcChatList();

    LineageRepresentation getRepresentation();
    DWORD getSibBase() const;

    void lockArrays();
    void unlockArrays();

private:
    DWORD getArraysAddress();
    DWORD getArraysNum();
    DWORD getADDR1();

    void uiElementAction(DWORD actionID, DWORD uiElementAddress);

    static LineageGlobal *_instance;
    static DWORD doActionOnInstanceFunction;
    static DWORD doActionOnInstanceECXArgument;
    static DWORD _moveToFunc;

    HANDLE _arraysMutex;

    DWORD _sibBase;

    std::vector<Mob> _mobs;
    std::vector<DroppedItem> _droppedItems;
    Macro macro;

    FDoAction _doAction;
    FUseSkill _useSkill;
};

#endif // LINEAGEGLOBAL_H
