#ifndef SKILL_H
#define SKILL_H

#include "instance.h"
#include "skillrepresentation.h"
#include <vector>

class Skill : public Instance
{
public:
    Skill(DWORD address);

    DWORD getID();
    DWORD getLevel();

    void makeRepresentation(SkillRepresentation &skillRep);

    static std::vector<Skill> &getActiveSkills();
    static std::vector<Skill> &getPassiveSkills();

private:
    static std::vector<Skill> passiveSkills;
    static std::vector<Skill> activeSkills;
};

#endif // SKILL_H
