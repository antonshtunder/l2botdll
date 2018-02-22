#ifndef SKILLLIST_H
#define SKILLLIST_H

#include "instance.h"

class SkillList : public Instance
{
public:
    static SkillList getActiveSkillList();
    static SkillList getPassiveSkillList();
private:
    SkillList(DWORD address, DWORD typesNum);

    DWORD _typesNum;
};

#endif // SKILLLIST_H
