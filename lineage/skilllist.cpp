#include "skilllist.h"
#include "lineageglobal.h"

SkillList::SkillList(DWORD address, DWORD typesNum):
    Instance(address)
{
    _typesNum = typesNum;
}

SkillList SkillList::getActiveSkillList()
{
    DWORD address = LineageGlobal::instance()->getSibBase() + 0x104;
    return SkillList(address, 8);
}

SkillList SkillList::getPassiveSkillList()
{
    DWORD address = LineageGlobal::instance()->getSibBase() + 0x14;
    return SkillList(address, 6);
}
