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
    void makeRepresentation(CharacterRepresentation &charRep);
};

#endif // PLAYER_H
