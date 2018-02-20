#include "instance.h"

Instance::Instance(DWORD address)
{
    _address = address;
}

DWORD Instance::address() const
{
    return _address;
}
