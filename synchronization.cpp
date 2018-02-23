#include "synchronization.h"
#include "dllhacklibrary.h"
#include "lineage/lineageglobal.h"

using namespace dhl;

#define MAIN_SLEEP_ADDRESS 0x74FB2870

static DWORD returnAddress = MAIN_SLEEP_ADDRESS + 5;
//04D84B8C  C01 + AC4B8C

static DWORD sleepArg;

void __stdcall mySleep(DWORD duration)
{
    if(duration != 1)
        return;
    auto l2g = LineageGlobal::instance();
    if(l2g->isInGame())
    {
        //l2g->refreshArrays();
    }
    //Sleep(duration);
}

void __declspec(naked) mySleepTrampoline()
{
    __asm
    {
        mov eax, dword ptr ss:[esp + 0x4];
        pushfd;
        pushad;
        push eax;
        call mySleep;
        popad;
        popfd;
        mov edi, edi;
        push ebp;
        mov ebp, esp;
        jmp returnAddress;
    }
}

void syncWithMainLoop()
{
    return;
    DWORD offset = 0;
    if(*reinterpret_cast<LPBYTE>(0x15033C98) == 0x90)
        ++offset;

    DWORD hookAddress = *reinterpret_cast<LPDWORD>(0x10BF10A4) + 0xAC4B8C;
    auto bytes = hookWithJump(MAIN_SLEEP_ADDRESS, reinterpret_cast<DWORD>(&mySleepTrampoline));
    delete [] bytes;
}
