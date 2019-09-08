#include "lineageglobal.h"
#include "dllhacklibrary.h"
#include "effect.h"
#include <Windows.h>
#include <set>
#include "synchronization.h"
#include "winfuncs.h"
#include <lineage/instances/item.h>

using namespace std;
using namespace dhl;

#define THREADSTACK0 0x19FF8C
#define MYLOCADDRESS 0x1510B080
#define ASTERIOSGAME 0x10900000

#define DO1 0x1094346C
#define CO1 0x10bf10a4


LineageGlobal *LineageGlobal::_instance = NULL;
DWORD LineageGlobal::doActionOnInstanceFunction = 0;
DWORD LineageGlobal::doActionOnInstanceECXArgument = 0;
DWORD LineageGlobal::_moveToFunc = 0;
DWORD LineageGlobal::_useItemFunc = 0;

static DWORD _esp;

LineageGlobal::LineageGlobal()
{
    DWORD funcBase = *reinterpret_cast<LPDWORD>(
                *reinterpret_cast<LPDWORD>(CO1)+ 0x718A0);
    _arraysMutex = CreateMutex(NULL, FALSE, NULL);
    _mobsMutex = CreateMutex(NULL, FALSE, NULL);
    _droppedItemsMutex = CreateMutex(NULL, FALSE, NULL);
    _doAction = reinterpret_cast<FDoAction>(funcBase + 0x512F60);
    _useSkill = reinterpret_cast<FUseSkill>(funcBase + 0x49AAE0);
    _moveToFunc = 0x203264F0;
    _useItemFunc = funcBase + 0x4c56C0;

    LineageGlobal::doActionOnInstanceFunction = 0x20460950;
    LineageGlobal::doActionOnInstanceECXArgument = *reinterpret_cast<LPDWORD>(
                *reinterpret_cast<LPDWORD>(0x19F020) + 0x68);

    BYTE skillInvBytes[] = {0x01, 0x00, 0x00, 0x00, 0x21, 0x00, 0x00, 0x00, 0x53, 0x00, 0x6B, 0x00, 0x69, 0x00, 0x6C, 0x00, 0x6C, 0x00, 0x00, 0x00};
    _sibBase = findByteSequence(skillInvBytes, sizeof(skillInvBytes), 0x15000000, 0x50000000);
}

LineageGlobal::~LineageGlobal()
{
    CloseHandle(_mobsMutex);
    CloseHandle(_droppedItemsMutex);
}

void __declspec(naked) moveFuncTrampoline()
{
    __asm
    {
        push ebp;
        lea ebp, dword ptr ss:[esp - 0x70];
        sub esp, 0x70;
        push 0xFFFFFFFF;
        push 0x204E3A8D;
        mov eax, dword ptr fs:[0];
        push eax;
        mov dword ptr fs:[0], esp;
        sub esp, 0x100;
        jmp LineageGlobal::_moveToFunc;
    }
}

DWORD getGUIBase()
{
    return *reinterpret_cast<LPDWORD>(*reinterpret_cast<LPDWORD>(THREADSTACK0 - 0xF6C) + 0x70);
}

DWORD getMoveToFuncEsi()
{
    return *reinterpret_cast<LPDWORD>(*reinterpret_cast<LPDWORD>(
           *reinterpret_cast<LPDWORD>(THREADSTACK0 - 0x604) + 0x7CC) + 0x3C);
}

DWORD getMoveToFuncEdi()
{
    return *reinterpret_cast<LPDWORD>(*reinterpret_cast<LPDWORD>(
           *reinterpret_cast<LPDWORD>(THREADSTACK0 - 0x604) + 0x7CC) + 0x78) + 0x34;
}

DWORD getCoordinatesAddress()
{
    return *reinterpret_cast<LPDWORD>(0x208C53E4);
}

Point3F *getMouseGeoCoordinates()
{
    return reinterpret_cast<Point3F*>(getCoordinatesAddress() + 0x254);
}

LineageGlobal *LineageGlobal::instance()
{
    if(_instance == NULL)
    {
        _instance = new LineageGlobal();
    }
    return _instance;
}

LPDWORD LineageGlobal::getPlayerTargetModelPointer()
{
    DWORD first = *reinterpret_cast<LPDWORD>(0x2185ea8c);
    if(!checkAddress(first))
        return reinterpret_cast<LPDWORD>(0);
    DWORD second = *reinterpret_cast<LPDWORD>(first + 0x3C);
    return reinterpret_cast<LPDWORD>(second + 0x450);
}

bool LineageGlobal::isInGame()
{
    DWORD indicator = *reinterpret_cast<LPDWORD>(0x2185ea8c);
    return (indicator == 0) ? false : true;
}

bool LineageGlobal::isDroppedItemPresent(DWORD address)
{
    auto items = lockDroppedItems();
    //lockArrays();
    for(auto item : _droppedItems)
    {
        if(item.address() == address)
        {
            //unlockArrays();
            unlockDroppedItems();
            return true;
        }
    }
    unlockDroppedItems();
    //unlockArrays();
    return false;
}

Player LineageGlobal::getPlayer()
{
    return Player();
}

std::vector<Mob> &LineageGlobal::lockMobs()
{
    WaitForSingleObject(_mobsMutex, INFINITE);
    //lockArrays();
    set<DWORD> mobAddresses;
    _mobs.clear();

    DWORD arrays = getArraysAddress();
    if(arrays < 0x20000)
        return _mobs;

    int i = 0;
    DWORD array, indicator, address;
    DWORD max = getArraysNum();
    for(DWORD i = 0; i < max; ++i)
    {
        array = *reinterpret_cast<LPDWORD>(arrays + 8 + i * 12);
        if(!checkAddress(array))
            break;
        int j = 0;
        while(true)
        {
            address = *reinterpret_cast<LPDWORD>(array + 4 + j * 8);
            if(address > 0x20000)
            {
                indicator = *reinterpret_cast<LPDWORD>(array + j * 8);
                ++j;
                if(indicator == 1)
                {
                    mobAddresses.insert(address);
                }
                else
                    break;
            }
            else
                break;
        }
        if(max != getArraysNum() || i >= max)
        {
            i = 0;
            mobAddresses.clear();
            max = getArraysNum();
        }
    }
    for(auto mob : mobAddresses)
    {
        _mobs.push_back(Mob(mob));
    }
    //unlockArrays();
    return _mobs;
}

std::vector<DroppedItem> &LineageGlobal::lockDroppedItems()
{
    WaitForSingleObject(_droppedItemsMutex, INFINITE);
    //lockArrays();
    set<DWORD> dropppedItemAddresses;
    _droppedItems.clear();

    DWORD arrays = getArraysAddress();
    if(arrays < 0x20000)
        return _droppedItems;;

    int i = 0;
    DWORD array, indicator, address;
    DWORD max = getArraysNum();
    for(DWORD i = 0; i < max; ++i)
    {
        array = *reinterpret_cast<LPDWORD>(arrays + 8 + i * 12);
        if(!checkAddress(array))
            break;
        int j = 0;
        while(true)
        {
            address = *reinterpret_cast<LPDWORD>(array + 4 + j * 8);
            if(address > 0x20000)
            {
                indicator = *reinterpret_cast<LPDWORD>(array + j * 8);
                ++j;
                if(indicator == 2)
                {
                    dropppedItemAddresses.insert(address);
                }
                else
                    break;
            }
            else
                break;
        }
        if(max != getArraysNum() || i >= max)
        {
            i = 0;
            dropppedItemAddresses.clear();
            max = getArraysNum();
        }
    }
    for(auto item : dropppedItemAddresses)
    {
        _droppedItems.push_back(DroppedItem(item));
    }
    //unlockArrays();
    return _droppedItems;
}

void LineageGlobal::unlockMobs()
{
    ReleaseMutex(_mobsMutex);
}

void LineageGlobal::unlockDroppedItems()
{
    ReleaseMutex(_droppedItemsMutex);
}

void LineageGlobal::attack()
{
    doAction(2);
}

void LineageGlobal::assist()
{
    doAction(5);
}

void LineageGlobal::performActionOn(DWORD instanceID)
{
    lockMutex();
    static DWORD _esp;
    __asm
    {
        pushfd;
        pushad;
        mov _esp, esp;

        mov ecx, LineageGlobal::doActionOnInstanceECXArgument;
        push instanceID;
        call LineageGlobal::doActionOnInstanceFunction;

        mov esp, _esp;
        popad;
        popfd;
    }
    releaseMutex();
}

void LineageGlobal::focusMob(DWORD address)
{
    Mob mob(address);
    *getPlayerTargetModelPointer() = mob.getModelAddress();
    attack();
}

void LineageGlobal::useSkill(DWORD skillID)
{
    macro.setUseSkill(skillID);
    lockMutex();
    _useSkill(macro.getAddress(), 0x0);
    releaseMutex();
}

void LineageGlobal::moveTo(float x, float y, float z)
{
    lockMutex();

    DWORD moveToEsi = getMoveToFuncEsi();
    DWORD moveToEdi = getMoveToFuncEdi();
    DWORD coordinatesStructure = getCoordinatesAddress();

    auto mouseCoordinates = getMouseGeoCoordinates();
    mouseCoordinates->x = x;
    mouseCoordinates->y = y;
    mouseCoordinates->z = z;

    __asm
    {
        pushfd;
        pushad;
        mov _esp, esp;

        mov esi, moveToEsi;
        mov edi, moveToEdi;
        mov ebx, coordinatesStructure;
        call moveFuncTrampoline;

        mov esp, _esp;
        popad;
        popfd;
    }

    releaseMutex();
}

void LineageGlobal::npcChat(DWORD index)
{
    DWORD chatList = getNpcChatList();
    DWORD listArray = *reinterpret_cast<LPDWORD>(chatList + 0x260);
    DWORD arraySize = *reinterpret_cast<LPDWORD>(chatList + 0x264);
    if(index >= arraySize)
        return;

    DWORD chatElement = *reinterpret_cast<LPDWORD>(*reinterpret_cast<LPDWORD>(listArray + index * 0xC + 0x8) + 0x254);
    //alert(numToStr(chatElement, 16));
    uiElementAction(0x201, chatElement);
}

void LineageGlobal::acceptAction()
{
    DWORD okButton = *reinterpret_cast<LPDWORD>(*reinterpret_cast<LPDWORD>(
                    *reinterpret_cast<LPDWORD>(*reinterpret_cast<LPDWORD>(
                    *reinterpret_cast<LPDWORD>(getGUIBase() + 0x6C) + 0x8) + 0x4)) + 0x21C);
    pushButton(okButton);
}

void LineageGlobal::useItem(DWORD id)
{
    DWORD useItemArg = getGUIBase();
    __asm
    {
        pushfd;
        pushad;
        mov _esp, esp;

        mov ecx, useItemArg;
        push id;
        call _useItemFunc;

        mov esp, _esp;
        popad;
        popfd;
    }
}

void LineageGlobal::test(DWORD address)
{
    uiElementAction(WM_LBUTTONDOWN, 0x233f2080);
    //pushButton(0x2413C300);
}

void LineageGlobal::pushButton(DWORD address)
{
    uiElementAction(WM_LBUTTONDOWN, address, 1);
    uiElementAction(WM_LBUTTONUP, address, 0);
}

void LineageGlobal::exchangeItem(DWORD id)
{
    DWORD base = *reinterpret_cast<LPDWORD>(getSibBase() - 0x190);
    DWORD itemsBase = *reinterpret_cast<LPDWORD>(*reinterpret_cast<LPDWORD>(base + 0x18) + 0x88);
    DWORD itemsNum = *reinterpret_cast<LPDWORD>(itemsBase + 0x290);
    if(id > itemsNum)
        return;
    LPDWORD choosenId = reinterpret_cast<LPDWORD>(itemsBase + 0x2CC);
    *choosenId = id;

    WORD buf[8] = {0};
    buf[0] = 49;
    DWORD inputBase = *reinterpret_cast<LPDWORD>(*reinterpret_cast<LPDWORD>(base + 0x2C) + 0x88);
    *reinterpret_cast<LPDWORD>(inputBase + 0x218) = reinterpret_cast<DWORD>(buf);
    *reinterpret_cast<LPDWORD>(inputBase + 0x21C) = 2;
    *reinterpret_cast<LPDWORD>(inputBase + 0x220) = 2;


    DWORD okButton = *reinterpret_cast<LPDWORD>(*reinterpret_cast<LPDWORD>(base + 0x24) + 0x88);
    pushButton(okButton);
    acceptExchange();
    *reinterpret_cast<LPDWORD>(inputBase + 0x218) = 0;
    *reinterpret_cast<LPDWORD>(inputBase + 0x21C) = 0;
    *reinterpret_cast<LPDWORD>(inputBase + 0x220) = 0;
}

void LineageGlobal::acceptExchange()
{
    DWORD confirmButton = *reinterpret_cast<LPDWORD>(*reinterpret_cast<LPDWORD>(
                            *reinterpret_cast<LPDWORD>(getSibBase() + 0xEE4) + 0x18) + 0x88);
    pushButton(confirmButton);
}

void LineageGlobal::addItemToPurchaseList(DWORD id, DWORD amount)
{
    DWORD itemsBase = *reinterpret_cast<LPDWORD>(*reinterpret_cast<LPDWORD>(
                    *reinterpret_cast<LPDWORD>(getSibBase() - 0xC34) + 0x1C) + 0x88);
    DWORD itemsNum = *reinterpret_cast<LPDWORD>(itemsBase + 0x290);
    LPDWORD itemsArray = *reinterpret_cast<LPDWORD*>(itemsBase + 0x28C);
    for(DWORD i = 0; i < itemsNum; ++i)
    {
        DWORD itemAddress = itemsArray[i];
        if(*reinterpret_cast<LPDWORD>(itemAddress + 0xFC) == id)
        {
            *reinterpret_cast<LPDWORD>(itemsBase + 0x2D0) = i;
            uiElementAction(WM_LBUTTONDBLCLK, itemsBase);

            DWORD inputAddress = *reinterpret_cast<LPDWORD>(*reinterpret_cast<LPDWORD>(
                                *reinterpret_cast<LPDWORD>(getSibBase() + 0xEE4) + 0x8) + 0x88);
            char *asciiNum = numToStr(amount, 10);
            short buffer[16];
            auto len = strlen(asciiNum);

            DWORD inputButtonsBase = *reinterpret_cast<LPDWORD>(getSibBase() + 0xED8);
            for(size_t i = 0; i < len; ++i)
            {
                DWORD inputBtnIndex = asciiNum[i] - '0';
                if(inputBtnIndex == 0)
                    inputBtnIndex = 9;
                else
                    --inputBtnIndex;
                DWORD inputButtonAddress = *reinterpret_cast<LPDWORD>(*reinterpret_cast<LPDWORD>
                                            (inputButtonsBase + 0x4 * inputBtnIndex) + 0x88);
                pushButton(inputButtonAddress);
            }
            /*buffer[len] = 0;
            *reinterpret_cast<LPDWORD>(inputAddress + 0x270) = 4 + len * 7;
            ++len; //to account for string terminator
            *reinterpret_cast<LPDWORD>(inputAddress + 0x218) = reinterpret_cast<DWORD>(buffer);
            *reinterpret_cast<LPDWORD>(inputAddress + 0x21C) = len;
            *reinterpret_cast<LPDWORD>(inputAddress + 0x220) = len;
            Sleep(5000);*/
            acceptExchange();
            return;
        }
    }

}

void LineageGlobal::confirmShopAction()
{
    DWORD buySellBtn = *reinterpret_cast<LPDWORD>(*reinterpret_cast<LPDWORD>(
                    *reinterpret_cast<LPDWORD>(getSibBase() - 0xC34) + 0x90) + 0x88);
    pushButton(buySellBtn);
}

DWORD LineageGlobal::getNpcChatList()
{
    return *reinterpret_cast<LPDWORD>(*reinterpret_cast<LPDWORD>(*reinterpret_cast<LPDWORD>(getSibBase() + 0x44C) + 0x4) + 0x88);
}

LineageRepresentation LineageGlobal::getRepresentation()
{
    LineageRepresentation representation;
    if(!isInGame())
        return representation;
    MobRepresentation mobRep;
    DroppedItemRepresentation droppedItemRep;
    ItemRepresentation itemRep;
    SkillRepresentation skillRep;
    EffectRepresentation effectRep;
    if(!getPlayer().makeRepresentation(representation.character))
        return representation;
    for(auto mob : lockMobs())
    {
        if(!mob.isValid())
            continue;
        mob.makeRepresentation(mobRep);
        representation.mobs.push_back(mobRep);
    }
    unlockMobs();
    for(auto item : lockDroppedItems())
    {
        if(!item.isValid())
            continue;
        item.makeRepresentation(droppedItemRep);
        representation.droppedItems.push_back(droppedItemRep);
    }
    unlockDroppedItems();
    for(auto skill : Skill::getActiveSkills())
    {
        skill.makeRepresentation(skillRep);
        representation.activeSkills.push_back(skillRep);
    }
    for(auto skill : Skill::getPassiveSkills())
    {
        skill.makeRepresentation(skillRep);
        representation.passiveSkills.push_back(skillRep);
    }
    for(auto effect : Effect::getPlayerEffects())
    {
        effect.makeRepresentation(effectRep);
        representation.playerEffects.push_back(effectRep);
    }
    for(auto effect : Effect::getTargetEffects())
    {
        effect.makeRepresentation(effectRep);
        representation.targetEffects.push_back(effectRep);
    }
    for(auto item : Item::getInventoryItems())
    {
        item.makeRepresentation(itemRep);
        representation.items.push_back(itemRep);
    }
    return representation;
}

DWORD LineageGlobal::getArraysAddress()
{
    DWORD first = *reinterpret_cast<LPDWORD>(THREADSTACK0-0x00000F6C);
    DWORD second = *reinterpret_cast<LPDWORD>(first + 0x68);
    if(!checkAddress(second))
        return 0;
    return *reinterpret_cast<LPDWORD>(second + 0x224);
}

DWORD LineageGlobal::getArraysNum()
{
    DWORD first = *reinterpret_cast<LPDWORD>(THREADSTACK0-0x00000F6C);
    DWORD second = *reinterpret_cast<LPDWORD>(first + 0x68);
    if(!checkAddress(second))
        return 0;
    return *reinterpret_cast<LPDWORD>(second + 0x228);
}

DWORD LineageGlobal::getADDR1()
{
    return *reinterpret_cast<LPDWORD>(0x208C53E8);
}

void LineageGlobal::uiElementAction(DWORD actionID, DWORD address, DWORD wParam, float xOffset, float yOffset)
{
    float elementX = *reinterpret_cast<float*>(address + 0xF4) + xOffset;
    float elementY = *reinterpret_cast<float*>(address + 0xF8) + yOffset;
    DWORD lParam = 0;

    outputDebug(numToStr(elementX, 10));
    outputDebug(numToStr(elementY, 10));
    outputDebug(numToStr(address, 16));
    outputDebug(numToStr(actionID, 16));

    *reinterpret_cast<short*>(reinterpret_cast<LPBYTE>(&lParam) + 0) = static_cast<short>(elementX) + 1;
    *reinterpret_cast<short*>(reinterpret_cast<LPBYTE>(&lParam) + 2) = static_cast<short>(elementY) + 1;
    __asm
    {
        pushfd;
        pushad;
        mov _esp, esp;

        mov ecx, address;
        push lParam;
        push wParam;
        push actionID;
        mov eax, dword ptr ds:[ecx];
        mov eax, dword ptr ds:[eax + 0x6C];
        call eax;

        mov esp, _esp;
        popad;
        popfd;
    }
}

void LineageGlobal::lockArrays()
{
    WaitForSingleObject(_arraysMutex, INFINITE);
}

void LineageGlobal::unlockArrays()
{
    ReleaseMutex(_arraysMutex);
}

void LineageGlobal::doAction(DWORD actionID)
{
    lockMutex();
    _doAction(actionID, getADDR1(), 0, 0);
    releaseMutex();
}

DWORD LineageGlobal::getSibBase() const
{
    return _sibBase;
}
