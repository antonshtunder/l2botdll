#ifndef EFFECT_H
#define EFFECT_H

#include "instance.h"
#include <vector>
#include "effectrepresentation.h"


class Effect : public Instance
{
public:
    Effect(DWORD address);

    DWORD getID();
    DWORD getLevel();
    DWORD getRemainingTime();
    bool isDebuff();

    void makeRepresentation(EffectRepresentation &skillRep);

    static std::vector<Effect> &getPlayerEffects();
    static std::vector<Effect> &getTargetEffects();

private:
    static std::vector<Effect> targetEffects;
    static std::vector<Effect> playerEffects;
};

#endif // EFFECT_H
