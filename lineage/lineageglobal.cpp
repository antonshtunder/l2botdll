#include "lineageglobal.h"
#include "dllhacklibrary.h"
#include <Windows.h>
#include <set>


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

LineageGlobal::LineageGlobal()
{
    _arraysMutex = CreateMutex(NULL, FALSE, NULL);
    _doAction = reinterpret_cast<FDoAction>(
                    *reinterpret_cast<LPDWORD>(
                        *reinterpret_cast<LPDWORD>(CO1)+ 0x718A0) + 0x512F60);

    LineageGlobal::doActionOnInstanceFunction = 0x20460950;
    LineageGlobal::doActionOnInstanceECXArgument = *reinterpret_cast<LPDWORD>(
                *reinterpret_cast<LPDWORD>(0x19F020) + 0x68);

    BYTE skillInvBytes[] = {0x01, 0x00, 0x00, 0x00, 0x21, 0x00, 0x00, 0x00, 0x53, 0x00, 0x6B, 0x00, 0x69, 0x00, 0x6C, 0x00, 0x6C, 0x00, 0x00, 0x00};
    _sibBase = findByteSequence(skillInvBytes, sizeof(skillInvBytes), 0x20000000, 0x40000000);
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
    auto items = getDroppedItems();
    lockArrays();
    for(auto item : _droppedItems)
    {
        if(item.address() == address)
        {
            unlockArrays();
            return true;
        }
    }
    unlockArrays();
    return false;
}

Player LineageGlobal::getPlayer()
{
    return Player();
}

std::vector<Mob> &LineageGlobal::getMobs()
{
    lockArrays();
    set<DWORD> mobAddresses;
    _mobs.clear();

    DWORD arrays = getArraysAddress();
    if(arrays < 0x20000)
        return _mobs;

    int i = 0;
    DWORD array, arrayIndicator, startId, indicator, address;
    while(true)
    {
        arrayIndicator = *reinterpret_cast<LPDWORD>(arrays + i * 12);
        if(arrayIndicator > 0x1000 && arrayIndicator != 0xFFFFFFFF)
            break;

        startId = *reinterpret_cast<LPDWORD>(arrays + 4 + i * 12);
        if(startId < 0x10000000 || startId > 0x40000000)
            break;

        array = *reinterpret_cast<LPDWORD>(arrays + 8 + i * 12);
        if(array < 0x20000 || array > 0x40000000)
            break;

        int j = 0;
        while(true)
        {
            address = *reinterpret_cast<LPDWORD>(array + 4 + j * 8);
            if(address > 0x20000 && address < 0x40000000)
            {
                indicator = *reinterpret_cast<LPDWORD>(array + j * 8);
                ++j;
                if(indicator == 1)
                {
                    mobAddresses.insert(address);
                }
                else if(indicator == 2)
                    continue;
                else
                    break;
            }
            else
                break;
        }
        ++i;
    }
    for(auto mob : mobAddresses)
    {
        _mobs.push_back(Mob(mob));
    }
    unlockArrays();
    return _mobs;
}

std::vector<DroppedItem> &LineageGlobal::getDroppedItems()
{
    lockArrays();
    set<DWORD> dropppedItemAddresses;
    _droppedItems.clear();

    DWORD arrays = getArraysAddress();
    if(arrays < 0x20000)
        return _droppedItems;;

    int i = 0;
    DWORD array, arrayIndicator, startId, indicator, address;
    while(true)
    {
        arrayIndicator = *reinterpret_cast<LPDWORD>(arrays + i * 12);
        if(arrayIndicator > 0x1000 && arrayIndicator != 0xFFFFFFFF)
            break;

        startId = *reinterpret_cast<LPDWORD>(arrays + 4 + i * 12);
        if(startId < 0x10000000 || startId > 0x40000000)
            break;

        array = *reinterpret_cast<LPDWORD>(arrays + 8 + i * 12);
        if(array < 0x20000 || array > 0x7fffffff)
            break;


        int j = 0;
        while(true)
        {
            address = *reinterpret_cast<LPDWORD>(array + 4 + j * 8);
            if(address > 0x20000 && address < 0x7fffffff)
            {
                indicator = *reinterpret_cast<LPDWORD>(array + j * 8);
                ++j;
                if(indicator == 2)
                {
                    dropppedItemAddresses.insert(address);
                }
                else if(indicator == 1)
                    continue;
                else
                    break;
            }
            else break;
        }
        ++i;
    }
    for(auto item : dropppedItemAddresses)
    {
        _droppedItems.push_back(DroppedItem(item));
    }
    unlockArrays();
    return _droppedItems;
}

void LineageGlobal::attack()
{
    doAction(2);
}

void LineageGlobal::performActionOn(DWORD instanceID)
{
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
}

void LineageGlobal::focusMob(DWORD address)
{
    Mob mob(address);
    *getPlayerTargetModelPointer() = mob.getModelAddress();
}

LineageRepresentation LineageGlobal::getRepresentation()
{
    LineageRepresentation representation;
    MobRepresentation mobRep;
    DroppedItemRepresentation itemRep;
    getPlayer().makeRepresentation(representation.character);
    for(auto mob : getMobs())
    {
        lockArrays();
        if(!mob.isValid())
            continue;
        mob.makeRepresentation(mobRep);
        representation.mobs.push_back(mobRep);
        unlockArrays();
    }

    for(auto item : getDroppedItems())
    {
        lockArrays();
        if(!item.isValid())
            continue;
        item.makeRepresentation(itemRep);
        representation.droppedItems.push_back(itemRep);
        unlockArrays();
    }
    return representation;
}

DWORD LineageGlobal::getArraysAddress()
{
    DWORD first = *reinterpret_cast<LPDWORD>(THREADSTACK0-0x00000F6C);
    DWORD second = *reinterpret_cast<LPDWORD>(first + 0x68);
    return *reinterpret_cast<LPDWORD>(second + 0x224);
}

DWORD LineageGlobal::getArraysNum()
{
    DWORD first = *reinterpret_cast<LPDWORD>(THREADSTACK0-0x00000F6C);
    DWORD second = *reinterpret_cast<LPDWORD>(first + 0x68);
    return second + 0x228;
}

DWORD LineageGlobal::getADDR1()
{
    return *reinterpret_cast<LPDWORD>(0x208C53E8);
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
    _doAction(actionID, getADDR1(), 0, 0);
}

DWORD LineageGlobal::getSibBase() const
{
    return _sibBase;
}
