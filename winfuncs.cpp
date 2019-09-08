#include <Windows.h>
#include "winfuncs.h"

#define NTDLL_PATH L"C:\\l2data\\ntdllc.dll"
#define KERNEL32_PATH L"C:\\l2data\\kernel32c.dll"
#define KERNELBASE_PATH L"C:\\l2data\\KernelBasec.dll"

struct SomeShit1
{
    DWORD blja1 = 0x18;
    DWORD blja2 = 0x0;
    DWORD blja3 = 0x0;
    DWORD blja4 = 0x0;
    DWORD blja5 = 0x0;
    DWORD blja6 = 0x0;
};

struct SomeShit2
{
    DWORD blja1 = 0x0;
    DWORD threadId = 0x0;

};

typedef NTSTATUS (WINAPI *FQit)(HANDLE th, DWORD tic, PVOID ti, ULONG til, PULONG rl);
typedef void (WINAPI *FZwOt)(PHANDLE pth, DWORD desiredAccess, SomeShit1 *someShit1, SomeShit2 *someShit2);
typedef void (WINAPI *FOutputDebug)(LPCSTR str);

static HMODULE ntDll;
static HMODULE kernel32;
static HMODULE kernelbase;

static FQit _NtQueryInformationThread;
static FZwOt _ZwOpenThread;
static FOutputDebug _OutputDebugStringA;

void initWinFuncs()
{
    ntDll = LoadLibraryW(NTDLL_PATH);
    kernel32 = LoadLibraryW(KERNEL32_PATH);
    kernelbase = LoadLibraryW(KERNELBASE_PATH);
    _NtQueryInformationThread = reinterpret_cast<FQit>(GetProcAddress(ntDll, "NtQueryInformationThread"));
    _ZwOpenThread = reinterpret_cast<FZwOt>(GetProcAddress(ntDll, "ZwOpenThread"));
    _OutputDebugStringA = reinterpret_cast<FOutputDebug>(GetProcAddress(kernelbase, "OutputDebugStringA"));
}

void outputDebug(LPCSTR str)
{
    _OutputDebugStringA(str);
}
