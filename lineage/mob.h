#ifndef MOB_H
#define MOB_H

#include "lineage/instance.h"
#include "lineagerepresentation.h"

class Mob : public Instance
{
public:
    Mob(DWORD address);

    int getHP() const;
    int getMaxHP() const;

    Point3F* getLoc() const;

    u16str getName() const;
    u16str getTitle() const;

    DWORD getID() const;
    DWORD getMobTypeID() const;
    DWORD getTitleColor() const;
    DWORD getModelAddress() const;

    void setAddress(DWORD newAddress);

    bool isMonster() const;
    bool isValid() const;
    bool isDead() const;
    bool isQuestMonster() const;
    bool isChampion() const;
    bool isPet() const;
    bool isNPC() const;

    static bool isDead(DWORD address);
    static bool isFocused(DWORD address);

    void makeRepresentation(MobRepresentation &mobRep);
};

#endif // MOB_H
