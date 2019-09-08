#include <Windows.h>
#include "lineage/lineageglobal.h"
#include <fstream>
#include "dllhacklibrary.h"
#include "synchronization.h"

using namespace dhl;

static DWORD patchReg;
static DWORD patchReg1;
static DWORD patchReg2;
static DWORD patchReg3;

#define PATCH_ADDRESS 0x150331A1//202729B3//20272A18
static DWORD returnAddress = PATCH_ADDRESS + 5;
static BYTE *patchOriginalBytes;
static DWORD patchOffset = 0;

static DWORD PATCH_ADDRESS1 = 0x0;
static DWORD returnAddress1 = 0x0;
static BYTE *patchOriginalBytes1;

#define PATCH_ADDRESS2 0x20273122
static DWORD returnAddress2 = PATCH_ADDRESS2 + 6;
static BYTE *patchOriginalBytes2;

#define PATCH_ADDRESS3 0x20272A99
static DWORD returnAddress3 = PATCH_ADDRESS3 + 5;
static BYTE *patchOriginalBytes3;

#define PATCH_ADDRESS4 0x202732B3
static DWORD returnAddress4 = PATCH_ADDRESS4 + 6;
static BYTE *patchOriginalBytes4;

static DWORD returnExtra = 0x20273B48;

std::ofstream outf("C:/patchFunc.txt");

DWORD getMyModel()
{
    return LineageGlobal::instance()->getPlayer().getModelAddress();
}

void __stdcall patchPrint(DWORD reg, DWORD n)
{
    outf << "reg" << n << "= " << (LPVOID)reg << std::endl;
}

static DWORD _sleepTime;

void __declspec(naked) patchFunction()
{
    __asm
    {
        mov eax, dword ptr ss:[esp];
        cmp eax, 10;
        ja sleepDurationTooLong;
        pushad;
        call sleepReleaseMutex;
        popad;
        call Sleep;
        pushad;
        call sleepLockMutex;
        popad;
        jmp returnAddress;

        sleepDurationTooLong:
        call Sleep;
        jmp returnAddress;
    }
}

static DWORD pf1eArg;

/*void __stdcall patchFunction1Extra()
{
    if(!checkAddress(pf1eArg))
}*/

DWORD getUnwantedThreadFunction()
{
    return *reinterpret_cast<LPDWORD>(0x10BF10A4) + 0x4A7B8;
}

void __declspec(naked) patchFunction1()
{
    __asm
    {
        call getUnwantedThreadFunction;
        cmp eax, dword ptr ss:[esp + 0x10];
        jne ok;
        ret 0x20;
        ok:
        push 0x270;
        jmp returnAddress1;
    }
}

void __declspec(naked) patchFunction2()
{
    __asm
    {
        cmp edx, 0;
        jne patchEnd;
        jmp returnExtra;
        mov edx, patchReg2;

        pushad;
        push 2;
        push edx;
        call patchPrint;
        popad;


        patchEnd:
        mov patchReg2, edx;
        cmp edi, dword ptr ds:[edx + 0x2CC];
        jmp returnAddress2;
    }
}

void __declspec(naked) patchFunction3()
{
    __asm
    {
        mov ecx, dword ptr ds:[ecx + 0xC];

        cmp ecx, 0;
        jne patchEnd;
        jmp returnExtra;
        mov ecx, patchReg3;

        pushad;
        push 3;
        push ecx;
        call patchPrint;
        popad;


        patchEnd:
        mov patchReg3, ecx;
        mov edx, dword ptr ds:[ecx];
        jmp returnAddress3;
    }
}

void __declspec(naked) patchFunction4()
{
    __asm
    {
        cmp edx, 0;
        jne patchEnd;
        jmp returnExtra;
        mov edx, patchReg2;

        pushad;
        push 4;
        push edx;
        call patchPrint;
        popad;


        patchEnd:
        mov patchReg3, edx;
        cmp edi, dword ptr ds:[edx + 0x2CC];
        jmp returnAddress4;
    }
}
//51C65DE0 | 3E 3B BA CC 02 00 00              | cmp edi,dword ptr ds:[edx+2CC]                                                            | abr.cpp:94



void applyPatches()
{
    if(readMemory<BYTE>(PATCH_ADDRESS) == 0x90)
        patchOffset = 1;
    returnAddress += patchOffset;
    patchOriginalBytes = hookWithJump(PATCH_ADDRESS + patchOffset, reinterpret_cast<DWORD>(&patchFunction));

    auto kernel32 = GetModuleHandleW(L"kernel32.dll");
    auto createRemoteThreadExAddress = GetProcAddress(kernel32, "CreateRemoteThreadEx");
    PATCH_ADDRESS1 = reinterpret_cast<DWORD>(createRemoteThreadExAddress);
    returnAddress1 = PATCH_ADDRESS1 + 5;
    patchOriginalBytes1 = hookWithJump(PATCH_ADDRESS1, reinterpret_cast<DWORD>(&patchFunction1));

    /*PATCH_ADDRESS1 = *reinterpret_cast<LPDWORD>(0x10BF10A4) + 0x7440;
    returnAddress1 = PATCH_ADDRESS1 + 6;
    patchOriginalBytes1 = hookWithJump(PATCH_ADDRESS1, reinterpret_cast<DWORD>(&patchFunction1));*/



    /*DWORD nopAddress = *reinterpret_cast<LPDWORD>(0x10BF10A4) + 0x7740;
    writeNOP(nopAddress, 2);*/

    /*nopAddress = *reinterpret_cast<LPDWORD>(0x10BF10A4) + 0x7439;
    writeNOP(nopAddress, 18);*/
    //writeMemory<BYTE>(nopAddress, 0xC3);
    //patchOriginalBytes = hookWithJump(PATCH_ADDRESS1, reinterpret_cast<DWORD>(&patchFunction1));
    /*patchOriginalBytes1 = hookWithJump(PATCH_ADDRESS1, reinterpret_cast<DWORD>(&patchFunction1));
    patchOriginalBytes2 = hookWithJump(PATCH_ADDRESS2, reinterpret_cast<DWORD>(&patchFunction2));
    patchOriginalBytes3 = hookWithJump(PATCH_ADDRESS3, reinterpret_cast<DWORD>(&patchFunction3));
    patchOriginalBytes4 = hookWithJump(PATCH_ADDRESS4, reinterpret_cast<DWORD>(&patchFunction4));*/
    //writeNOP(PATCH_ADDRESS2 + 5, 1);
}

void removePatches()
{
    unhookWithJump(PATCH_ADDRESS + patchOffset, patchOriginalBytes);
    unhookWithJump(PATCH_ADDRESS1, patchOriginalBytes1);
    Sleep(500);
}
