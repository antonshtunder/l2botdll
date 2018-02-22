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

bool LineageGlobal::isAddressInArray(DWORD address)
{
    DWORD arrays = getArraysAddress();
    if(arrays == 0)
        return false;

    int i = 0;
    DWORD startId, array, indicator, foundAddress;
    while(true)
    {
        startId = *reinterpret_cast<LPDWORD>(arrays + 4 + i * 12);

        if(startId < 0x10000000 || startId > 0x40000000)
            break;

        array = *reinterpret_cast<LPDWORD>(arrays + 8 + i * 12);
        int j = 0;
        while(true)
        {
            indicator = *reinterpret_cast<LPDWORD>(array + j * 8);

            if(indicator != 1 && indicator != 2)
                break;


            foundAddress = *reinterpret_cast<LPDWORD>(array + 4 + j * 8);
            if(foundAddress == 0)
            {
                break;
            }
            else if(foundAddress == address)
            {
                return true;
            }
            ++j;
        }

        ++i;
    }
    return false;
}

Player LineageGlobal::getPlayer()
{
    return Player();
}

std::vector<Mob> &LineageGlobal::getMobs()
{
    set<DWORD> mobAddresses;
    _mobs.clear();

    DWORD arrays = getArraysAddress();
    if(arrays == 0)
        return _mobs;
    int i = 0;
    DWORD startId, array, mobIndicator, mobAddress;
    while(true)
    {
        startId = *reinterpret_cast<LPDWORD>(arrays + 4 + i * 12);

        if(startId < 0x10000000 || startId > 0x40000000)
            break;

        array = *reinterpret_cast<LPDWORD>(arrays + 8 + i * 12);
        if(array < 0x20000 || array > 0x7fffffff)
            break;

        int j = 0;
        while(true)
        {
            mobIndicator = *reinterpret_cast<LPDWORD>(array + j * 8);

            if(mobIndicator != 1)
            {
                if(mobIndicator == 2)
                {
                    ++j;
                    continue;
                }
                else
                {
                    break;
                }
            }

            mobAddress = *reinterpret_cast<LPDWORD>(array + 4 + j * 8);
            if(mobAddress > 0x20000 && mobAddress < 0x7fffffff)
            {
                mobAddresses.insert(mobAddress);
            }
            ++j;
        }

        ++i;
    }
    for(auto begin = mobAddresses.cbegin(); begin != mobAddresses.end(); ++begin)
    {
        _mobs.push_back(Mob(*begin));
    }
    return _mobs;
}

std::vector<DroppedItem> &LineageGlobal::getDroppedItems()
{
    set<DWORD> itemAddresses;
    _droppedItems.clear();

    DWORD arrays = getArraysAddress();
    if(arrays == 0)
        return _droppedItems;

    int i = 0;
    DWORD startId, array, itemIndicator, itemAddress;
    while(true)
    {
        startId = *reinterpret_cast<LPDWORD>(arrays + 4 + i * 12);

        if(startId < 0x10000000 || startId > 0x40000000)
            break;

        array = *reinterpret_cast<LPDWORD>(arrays + 8 + i * 12);
        if(array < 0x20000 || array > 0x7fffffff)
            break;

        int j = 0;
        while(true)
        {
            itemIndicator = *reinterpret_cast<LPDWORD>(array + j * 8);
            if(itemIndicator != 2)
            {
                if(itemIndicator == 1)
                {
                    ++j;
                    continue;
                }
                else
                {
                    break;
                }
            }

            itemAddress = *reinterpret_cast<LPDWORD>(array + 4 + j * 8);
            ++j;
            if(itemAddress != 0)
            {
                if(!DroppedItem::isValid(itemAddress))
                    continue;
                itemAddresses.insert(itemAddress);
            }
        }

        ++i;
    }
    for(auto begin = itemAddresses.cbegin(); begin != itemAddresses.end(); ++begin)
    {
        _droppedItems.push_back(DroppedItem(*begin));
    }
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
        if(!mob.isValid())
            continue;
        mob.makeRepresentation(mobRep);
        representation.mobs.push_back(mobRep);
    }

    for(auto item : getDroppedItems())
    {
        if(!item.isValid())
            continue;
        item.makeRepresentation(itemRep);
        representation.droppedItems.push_back(itemRep);
    }
    return representation;
}

DWORD LineageGlobal::getArraysAddress()
{
    DWORD first = *reinterpret_cast<LPDWORD>(THREADSTACK0-0x00000F6C);
    DWORD second = *reinterpret_cast<LPDWORD>(first + 0x68);
    return *reinterpret_cast<LPDWORD>(second + 0x224);
}

DWORD LineageGlobal::getADDR1()
{
    return *reinterpret_cast<LPDWORD>(0x208C53E8);
}

void LineageGlobal::doAction(DWORD actionID)
{
    _doAction(actionID, getADDR1(), 0, 0);
}

DWORD LineageGlobal::getSibBase() const
{
    return _sibBase;
}
