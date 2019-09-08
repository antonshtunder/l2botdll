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
    ~LineageGlobal();

    static LineageGlobal *instance();
    static LPDWORD getPlayerTargetModelPointer();

    bool isInGame();
    bool isDroppedItemPresent(DWORD address);

    Player getPlayer();
    std::vector<Mob> &lockMobs();
    std::vector<DroppedItem> &lockDroppedItems();
    void unlockMobs();
    void unlockDroppedItems();

    void doAction(DWORD actionID);
    void attack();
    void assist();
    void performActionOn(DWORD instanceID);
    void focusMob(DWORD address);
    void useSkill(DWORD skillID);
    void moveTo(float x, float y, float z);
    void npcChat(DWORD index);
    void acceptAction();
    void useItem(DWORD id);
    void test(DWORD address);
    void pushButton(DWORD address);
    void exchangeItem(DWORD index);
    void acceptExchange();
    void addItemToPurchaseList(DWORD id, DWORD amount);
    void confirmShopAction();

    DWORD getNpcChatList();

    LineageRepresentation getRepresentation();
    DWORD getSibBase() const;

    void lockArrays();
    void unlockArrays();

private:
    DWORD getArraysAddress();
    DWORD getArraysNum();
    DWORD getADDR1();

    void uiElementAction(DWORD actionID, DWORD uiElementAddress, DWORD wParam = 0, float xOffset = 0.0f, float yOffset = 0.0f);

    static LineageGlobal *_instance;
    static DWORD doActionOnInstanceFunction;
    static DWORD doActionOnInstanceECXArgument;
    static DWORD _moveToFunc;
    static DWORD _useItemFunc;

    HANDLE _arraysMutex;

    DWORD _sibBase;

    std::vector<Mob> _mobs;
    std::vector<DroppedItem> _droppedItems;
    Macro macro;

    FDoAction _doAction;
    FUseSkill _useSkill;

    HANDLE _mobsMutex, _droppedItemsMutex;;
};

#endif // LINEAGEGLOBAL_H
