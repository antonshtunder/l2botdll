#include "skill.h"
#include "lineageglobal.h"

using namespace std;

vector<Skill> Skill::activeSkills;
vector<Skill> Skill::passiveSkills;

Skill::Skill(DWORD address):
    Instance(address)
{

}

DWORD Skill::getID()
{
    return *reinterpret_cast<LPDWORD>(_address + 0xFC);
}

DWORD Skill::getLevel()
{
    return *reinterpret_cast<LPDWORD>(_address + 0x100);
}

void Skill::makeRepresentation(SkillRepresentation &skillRep)
{
    skillRep.id = getID();
    skillRep.level = getLevel();
}

std::vector<Skill> &Skill::getActiveSkills()
{
    DWORD listAddress = LineageGlobal::instance()->getSibBase() + 0x104;
    DWORD numTypes = 8;
    for(DWORD i = 1; i <= numTypes; ++i)
    {
        DWORD firstAddress = *reinterpret_cast<LPDWORD>(listAddress - (numTypes - i) * 0x24);
        DWORD secondAddress = *reinterpret_cast<LPDWORD>(firstAddress + 0x4);
        DWORD thirdAddress = *reinterpret_cast<LPDWORD>(secondAddress + 0x88);
        DWORD skillsArray = *reinterpret_cast<LPDWORD>(thirdAddress + 0x28C);
        DWORD numSkills = *reinterpret_cast<LPDWORD>(secondAddress + 0x290);
        for(DWORD j = 0; j < numSkills; ++j)
        {
            activeSkills.push_back(Skill(*reinterpret_cast<LPDWORD>(skillsArray + j * 0x4)));
        }
    }
    return activeSkills;
}

std::vector<Skill> &Skill::getPassiveSkills()
{
    DWORD listAddress = LineageGlobal::instance()->getSibBase() + 0x14;
    DWORD numTypes = 8;
    for(DWORD i = 1; i <= numTypes; ++i)
    {
        DWORD firstAddress = *reinterpret_cast<LPDWORD>(listAddress - (numTypes - i) * 0x24);
        DWORD secondAddress = *reinterpret_cast<LPDWORD>(firstAddress + 0x4);
        DWORD thirdAddress = *reinterpret_cast<LPDWORD>(secondAddress + 0x88);
        DWORD skillsArray = *reinterpret_cast<LPDWORD>(thirdAddress + 0x28C);
        DWORD numSkills = *reinterpret_cast<LPDWORD>(secondAddress + 0x290);
        for(DWORD j = 0; j < numSkills; ++j)
        {
            passiveSkills.push_back(Skill(*reinterpret_cast<LPDWORD>(skillsArray + j * 0x4)));
        }
    }
    return passiveSkills;
}
