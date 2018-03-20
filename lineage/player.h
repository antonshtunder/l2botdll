#ifndef PLAYER_H
#define PLAYER_H

#include "utils.h"
#include "characterrepresentation.h"

class Player
{
public:
    Player();

    u16str getName();
    Point3F *getLoc();
    DWORD address() const;

    DWORD getHP();
    DWORD getMaxHP();
    DWORD getMP();
    DWORD getMaxMP();
    DWORD getCP();
    DWORD getMaxCP();
    DWORD getModelAddress();
    bool isCasting();

    void makeRepresentation(CharacterRepresentation &charRep);

private:
    DWORD getHPMPCPBase();
};

#endif // PLAYER_H
