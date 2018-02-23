#include "mob.h"
#include "lineage/lineageglobal.h"
#include "utils.h"

#define HP_OFFSET 0xB4
#define MAX_HP_OFFSET 0xB0
#define NAME_OFFSET 0x1C
#define TITLE_OFFSET 0x3C8
#define MODEL_ADDRESS_OFFSET 0x374
#define ID_OFFSET 0x18
#define MOBTYPE_ID_OFFSET 0x14
#define IS_MONSTER_OFFSET 0x10
#define IS_PET_OFFSET 0xC
#define IS_NPC_OFFSET 0x8
#define TITLE_COLOR_OFFSET 0x254
#define MODEL_LOC_OFFSET 0x1B4

Mob::Mob(DWORD address):
    Instance(address) {}

int Mob::getHP() const
{
    int *hpLoc = reinterpret_cast<int*>(_address + HP_OFFSET);
    return *hpLoc;
}

int Mob::getMaxHP() const
{
    int *maxHpLoc = reinterpret_cast<int*>(_address + MAX_HP_OFFSET);
    return *maxHpLoc;
}

Point3F* Mob::getLoc() const
{
    auto model = getModelAddress();
    return reinterpret_cast<Point3F*>(model + MODEL_LOC_OFFSET);
}

u16str Mob::getName() const
{
    return reinterpret_cast<u16str>(_address + NAME_OFFSET);
}

u16str Mob::getTitle() const
{
    return reinterpret_cast<u16str>(_address + TITLE_OFFSET);
}

DWORD Mob::getID() const
{
    LPDWORD idLoc = reinterpret_cast<LPDWORD>(_address + ID_OFFSET);
    return *idLoc;
}

DWORD Mob::getMobTypeID() const
{
    LPDWORD mobTypeIdLoc = reinterpret_cast<LPDWORD>(_address + MOBTYPE_ID_OFFSET);
    return *mobTypeIdLoc;
}

DWORD Mob::getTitleColor() const
{
    return *reinterpret_cast<LPDWORD>(_address + TITLE_COLOR_OFFSET);
}

DWORD Mob::getModelAddress() const
{
    return *reinterpret_cast<LPDWORD>(_address + MODEL_ADDRESS_OFFSET);
}

bool Mob::isQuestMonster() const
{
    return getTitleColor() == 0xFFFF8000;
}

bool Mob::isChampion() const
{
    return getMaxHP() > 15000;
}

bool Mob::isPet() const
{
    return *reinterpret_cast<bool*>(_address + IS_PET_OFFSET);
}

bool Mob::isNPC() const
{
    return *reinterpret_cast<bool*>(_address + IS_NPC_OFFSET);
}

bool Mob::isDead(DWORD address)
{
    Mob mob(address);
    if(mob.getModelAddress() == 0)
        return true;
    if(mob.getHP() == 0)
        return true;
    return false;
}

bool Mob::isFocused(DWORD address)
{
    Mob mob(address);
    return mob.getModelAddress() == *LineageGlobal::getPlayerTargetModelPointer();
}

void Mob::makeRepresentation(MobRepresentation &mobRep)
{
    memcpy(mobRep.name, getName(), 60);
    memcpy(mobRep.title, getTitle(), 60);
    mobRep.x = getLoc()->x;
    mobRep.y = getLoc()->y;
    mobRep.z = getLoc()->z;
    mobRep.address = _address;
    mobRep.hp = getHP();
    mobRep.id = getID();
    mobRep.maxHp = getMaxHP();
    mobRep.typeID = getMobTypeID();

    if(isPet())
        mobRep.mobType = MobType::PET;
    else if(isMonster())
        if(isQuestMonster())
            mobRep.mobType = MobType::QUEST_MONSTER;
        else
            mobRep.mobType = MobType::MONSTER;
    else if(isNPC())
        mobRep.mobType = MobType::NPC;
    else
        mobRep.mobType = MobType::PLAYER;
}

bool Mob::isMonster() const
{
    return *reinterpret_cast<bool*>(_address + IS_MONSTER_OFFSET);
}

bool Mob::isValid() const
{
    if(_address == 0)
        return false;

    if(!checkAddress(_address))
        return false;

    if(*reinterpret_cast<LPDWORD>(_address) != 0)
        return false;

    if(*reinterpret_cast<LPDWORD>(_address + 4) != 0)
        return false;

    if(getID() < 0x10000000 || getID() > 0x20000000)
        return false;

    if(getModelAddress() == 0)
        return false;

    return true;
}

bool Mob::isDead() const
{
    return Mob::isDead(address());
}
