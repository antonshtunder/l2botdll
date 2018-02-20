#ifndef INSTANCE_H
#define INSTANCE_H

#include <Windows.h>
#include "utils.h"

class Instance
{
public:
    Instance(DWORD address);

    DWORD address() const;

protected:
    DWORD _address;
};

#endif // INSTANCE_H
