#include "player.h"
#include "lineageglobal.h"
#include "dllhacklibrary.h"

DWORD Player::address() const
{
    return *reinterpret_cast<LPDWORD>(
            *reinterpret_cast<LPDWORD>(
                    *reinterpret_cast<LPDWORD>(
                        *reinterpret_cast<LPDWORD>(0x109D9588)
                        + 0x1294)) + 0x988);
}

DWORD Player::getHP()
{
    return *reinterpret_cast<LPDWORD>(*reinterpret_cast<LPDWORD>(
            *reinterpret_cast<LPDWORD>(getHPMPCPBase() + 0xC) + 0x88) + 0x25c);
}

DWORD Player::getMaxHP()
{
    return *reinterpret_cast<LPDWORD>(*reinterpret_cast<LPDWORD>(
            *reinterpret_cast<LPDWORD>(getHPMPCPBase() + 0xC) + 0x88) + 0x254);
}

DWORD Player::getMP()
{
    return *reinterpret_cast<LPDWORD>(*reinterpret_cast<LPDWORD>(
            *reinterpret_cast<LPDWORD>(getHPMPCPBase() + 0x10) + 0x88) + 0x25c);
}

DWORD Player::getMaxMP()
{
    return *reinterpret_cast<LPDWORD>(*reinterpret_cast<LPDWORD>(
            *reinterpret_cast<LPDWORD>(getHPMPCPBase() + 0x10) + 0x88) + 0x254);
}

DWORD Player::getCP()
{
    return *reinterpret_cast<LPDWORD>(*reinterpret_cast<LPDWORD>(
            *reinterpret_cast<LPDWORD>(getHPMPCPBase() + 0x8) + 0x88) + 0x25c);
}

DWORD Player::getMaxCP()
{
    return *reinterpret_cast<LPDWORD>(*reinterpret_cast<LPDWORD>(
            *reinterpret_cast<LPDWORD>(getHPMPCPBase() + 0x8) + 0x88) + 0x254);
}

DWORD Player::getModelAddress()
{
    DWORD base = reinterpret_cast<DWORD>(GetModuleHandleW(L"awesomium.DLL"));
    return *reinterpret_cast<LPDWORD>(*reinterpret_cast<LPDWORD>(
            *reinterpret_cast<LPDWORD>(base + 0x11C9AB8) + 0x34) + 0x24);
}

bool Player::isCasting()
{
    return (*reinterpret_cast<LPDWORD>(getModelAddress() + 0x568) > 0);
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
    auto targetPointer = LineageGlobal::getPlayerTargetModelPointer();
    if(!dhl::validateAddress(reinterpret_cast<DWORD>(targetPointer)))
    {
        charRep.targetModelAddress = 0;
    }
    else
    {
        charRep.targetModelAddress = *targetPointer;
    }
    charRep.hp = getHP();
    charRep.maxHp = getMaxHP();
    charRep.mp = getMP();
    charRep.maxMp = getMaxMP();
    charRep.cp = getCP();
    charRep.maxCp = getMaxCP();
}

DWORD Player::getHPMPCPBase()
{
    return*reinterpret_cast<LPDWORD>(
            LineageGlobal::instance()->getSibBase() - 0xDCC);
}
