#ifndef MACRO_H
#define MACRO_H

#include <Windows.h>

class Macro
{
public:
    Macro();

    DWORD getAddress();
    void addUseSkill(DWORD skillId);
    void setUseSkill(DWORD skillId);
private:
    BYTE _data[4096];

    size_t *n;
};

#endif // MACRO_H
