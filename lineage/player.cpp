#include "player.h"


DWORD Player::address() const
{
    return *reinterpret_cast<LPDWORD>(
            *reinterpret_cast<LPDWORD>(
                    *reinterpret_cast<LPDWORD>(
                        *reinterpret_cast<LPDWORD>(0x109D9588)
                        + 0x1294)) + 0x988);
}

Player::Player()
{ }

u16str Player::getName()
{
    return reinterpret_cast<u16str>(address() + 0x1C);
}

Point3F *Player::getLoc()
{
    return reinterpret_cast<Point3F*>(0x1510B080);
}

void Player::makeRepresentation(CharacterRepresentation &charRep)
{
    memcpy(charRep.name, getName(), 60);
    charRep.x = getLoc()->x;
    charRep.y = getLoc()->y;
    charRep.z = getLoc()->z;
    charRep.address = address();
    charRep.hp = 5;
    charRep.mp = 5;
}
