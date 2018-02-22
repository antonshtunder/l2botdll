#ifndef EFFECTLIST_H
#define EFFECTLIST_H

#include "instance.h"



class EffectList : public Instance
{
public:
    static EffectList getPlayerEffects();
    static EffectList getTargetEffects();
private:
    EffectList(DWORD address);
};

#endif // EFFECTLIST_H
