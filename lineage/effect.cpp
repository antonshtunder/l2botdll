#include "effect.h"
#include "lineageglobal.h"

std::vector<Effect> Effect::playerEffects;
std::vector<Effect> Effect::targetEffects;

DWORD getPlayerEffectsBase()
{
    DWORD base = LineageGlobal::instance()->getSibBase();
    if(base < 0x200000)
        return 0;

    DWORD secondBase = *reinterpret_cast<LPDWORD>(
                base + 0x1484);

    MEMORY_BASIC_INFORMATION mbi;
    VirtualQuery(reinterpret_cast<LPVOID>(secondBase), &mbi, sizeof(mbi));
    if(mbi.State != MEM_COMMIT)
        secondBase = *reinterpret_cast<LPDWORD>(
                        base + 0x149C);

    return (*reinterpret_cast<LPDWORD>(
            *reinterpret_cast<LPDWORD>(
            secondBase) + 0x88) + 0x220);
}

DWORD getTargetEffectsBase()
{
    DWORD base = LineageGlobal::instance()->getSibBase();
    if(base < 0x200000)
        return 0;
    return (*reinterpret_cast<LPDWORD>(
            *reinterpret_cast<LPDWORD>(
            *reinterpret_cast<LPDWORD>(
            base - 0xF10) + 0x8) + 0x88) + 0x220);
}

void getEffects(DWORD base, std::vector<Effect> &effects)
{
    if(base < 0x200000)
        return;

    LPDWORD effectTypeArray = *reinterpret_cast<LPDWORD*>(base);
    DWORD effectTypeArrayNum = *reinterpret_cast<LPDWORD>(base + 0x4);
    for(DWORD i = 0; i < effectTypeArrayNum; ++i)
    {
        DWORD effectTypeBase = effectTypeArray[i];
        LPDWORD effectArray = *reinterpret_cast<LPDWORD*>(effectTypeBase);
        DWORD effectArrayNum = *reinterpret_cast<LPDWORD>(effectTypeBase + 0x4);
        for(DWORD j = 0; j < effectArrayNum; ++j)
        {
            effects.push_back(effectArray[j]);
        }
    }
}

Effect::Effect(DWORD address):
    Instance(address)
{

}

DWORD Effect::getID()
{
    return *reinterpret_cast<LPDWORD>(_address + 0xFC);
}

DWORD Effect::getLevel()
{
    return *reinterpret_cast<LPDWORD>(_address + 0x100);
}

DWORD Effect::getRemainingTime()
{
    return *reinterpret_cast<LPDWORD>(_address + 0x124);
}

bool Effect::isDebuff()
{
    return *reinterpret_cast<bool*>(_address + 0x2BC);
}

void Effect::makeRepresentation(EffectRepresentation &skillRep)
{
    skillRep.id = getID();
    skillRep.level = getLevel();
    skillRep.remainingTime = getRemainingTime();
    skillRep.isDebuff = isDebuff();
}

std::vector<Effect> &Effect::getPlayerEffects()
{
    playerEffects.clear();
    DWORD base = getPlayerEffectsBase();
    getEffects(base, playerEffects);
    return playerEffects;
}

std::vector<Effect> &Effect::getTargetEffects()
{
    targetEffects.clear();
    DWORD base = getTargetEffectsBase();
    getEffects(base, targetEffects);
    return targetEffects;
}
