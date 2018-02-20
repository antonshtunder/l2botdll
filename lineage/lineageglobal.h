#ifndef LINEAGEGLOBAL_H
#define LINEAGEGLOBAL_H

#include "player.h"
#include "mob.h"
#include "lineagerepresentation.h"

class LineageGlobal
{
public:
    LineageGlobal();

    static LineageGlobal *instance();

    bool isInGame();

    Player getPlayer();
    std::vector<Mob> &getMobs();

    LineageRepresentation getRepresentation();
private:
    static LineageGlobal *_instance;
    std::vector<Mob> mobs;

    DWORD getArraysAddress();
};

#endif // LINEAGEGLOBAL_H
