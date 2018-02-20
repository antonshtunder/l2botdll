#include "lineageglobal.h"
#include <Windows.h>
#include <set>

using namespace std;

#define THREADSTACK0 0x19FF8C

LineageGlobal *LineageGlobal::_instance = NULL;

LineageGlobal::LineageGlobal()
{

}

LineageGlobal *LineageGlobal::instance()
{
    if(_instance == NULL)
    {
        _instance = new LineageGlobal();
    }
    return _instance;
}

bool LineageGlobal::isInGame()
{
    DWORD indicator = *reinterpret_cast<LPDWORD>(0x2185ea8c);
    return (indicator == 0) ? false : true;
}

Player LineageGlobal::getPlayer()
{
    return Player();
}

std::vector<Mob> &LineageGlobal::getMobs()
{
    set<DWORD> mobAddresses;
    mobs.clear();

    DWORD mobArrays = getArraysAddress();
    int i = 0;
    DWORD startId, array, mobIndicator, mobAddress;
    while(true)
    {
        startId = *reinterpret_cast<LPDWORD>(mobArrays + 4 + i * 12);

        if(startId < 0x10000000 || startId > 0x20000000)
            break;

        array = *reinterpret_cast<LPDWORD>(mobArrays + 8 + i * 12);
        int j = 0;
        while(true)
        {
            mobIndicator = *reinterpret_cast<LPDWORD>(array + j * 8);
            if(mobIndicator != 1)
                break;

            mobAddress = *reinterpret_cast<LPDWORD>(array + 4 + j * 8);
            if(mobAddress != 0)
            {
                mobAddresses.insert(mobAddress);
            }
            ++j;
        }

        ++i;
    }
    for(auto begin = mobAddresses.cbegin(); begin != mobAddresses.end(); ++begin)
    {
        mobs.push_back(Mob(*begin));
    }
    return mobs;
}

LineageRepresentation LineageGlobal::getRepresentation()
{
    LineageRepresentation representation;
    getPlayer().makeRepresentation(representation.character);
    for(auto mob : getMobs())
    {
        if(!mob.isValid())
            continue;
        if(mob.address() == getPlayer().address())
            continue;
        MobRepresentation mobRep;
        mob.makeRepresentation(mobRep);
        representation.mobs.push_back(mobRep);
    }
    return representation;
}

DWORD LineageGlobal::getArraysAddress()
{
    DWORD first = *reinterpret_cast<LPDWORD>(THREADSTACK0-0x00000F6C);
    DWORD second = *reinterpret_cast<LPDWORD>(first + 0x68);
    return *reinterpret_cast<LPDWORD>(second + 0x224);
}
