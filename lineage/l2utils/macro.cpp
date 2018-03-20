#include "macro.h"

Macro::Macro()
{
    ZeroMemory(_data, sizeof(_data));
    _data[0x28] = 1;
    n = reinterpret_cast<size_t*>(_data + 0xE24);
}

DWORD Macro::getAddress()
{
    return reinterpret_cast<DWORD>(_data);
}

void Macro::addUseSkill(DWORD skillId)
{
    size_t offset = 0x30 + *n * 0xD0;
    BYTE *base = &_data[offset];
    *(base + 4) = 1;
    *(reinterpret_cast<LPDWORD>(base + 0xC)) = skillId;
    *(base + 0x14) = 1;
    (*n)++;
}

void Macro::setUseSkill(DWORD skillId)
{
    size_t offset = 0x30;
    BYTE *base = &_data[offset];
    *(base + 4) = 1;
    *(reinterpret_cast<LPDWORD>(base + 0xC)) = skillId;
    *n = 1;
}
