#include "synchronization.h"
#include "dllhacklibrary.h"
#include "lineage/lineageglobal.h"
#include "winternl.h"
#include <sstream>
#include "winfuncs.h"


using namespace dhl;

#define MAIN_SLEEP_ADDRESS 0x74FB2870

static DWORD returnAddress = MAIN_SLEEP_ADDRESS + 5;
//04D84B8C  C01 + AC4B8C

static DWORD sleepArg;


static HANDLE mutex;
static HANDLE sleepMutex;

static HANDLE sleepEvent;
static HANDLE actionDoneEvent;

std::vector<THREADENTRY32> getProcessThreads()
{
    HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    std::vector<THREADENTRY32> threads;
    if (h != INVALID_HANDLE_VALUE)
    {
        THREADENTRY32 te;
        te.dwSize = sizeof(te);
        if (Thread32First(h, &te))
        {
            do
            {
                if (te.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) + sizeof(te.th32OwnerProcessID))
                {
                    if(te.th32OwnerProcessID == GetCurrentProcessId())
                    {
                        threads.push_back(te);
                    }
                }
                te.dwSize = sizeof(te);
            } while (Thread32Next(h, &te));
        }
        CloseHandle(h);
    }

    return threads;
}

void closeSynchronizationHandles()
{
    CloseHandle(mutex);
    CloseHandle(sleepMutex);
    CloseHandle(sleepEvent);
    CloseHandle(actionDoneEvent);
}

void lockMutex()
{
    WaitForSingleObject(sleepEvent, INFINITE);
    ResetEvent(actionDoneEvent);
    return;
    WaitForSingleObject(mutex, 100);
}

void releaseMutex()
{
    SetEvent(actionDoneEvent);
    return;
    ReleaseMutex(mutex);
}

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

void initSynchronization()
{
    mutex = CreateMutex(NULL, FALSE, NULL);
    sleepMutex = CreateMutex(NULL, FALSE, NULL);
    sleepEvent = CreateEvent(NULL, TRUE, FALSE, TEXT("sleepEvent"));
    actionDoneEvent = CreateEvent(NULL, TRUE, TRUE, TEXT("actionDoneEvent"));
    return;
    DWORD offset = 0;
    if(*reinterpret_cast<LPBYTE>(0x15033C98) == 0x90)
        ++offset;

    DWORD hookAddress = *reinterpret_cast<LPDWORD>(0x10BF10A4) + 0xAC4B8C;
    auto bytes = hookWithJump(MAIN_SLEEP_ADDRESS, reinterpret_cast<DWORD>(&mySleepTrampoline));
    delete [] bytes;
}

void sleepLockMutex()
{
    WaitForSingleObject(actionDoneEvent, 500);
    ResetEvent(sleepEvent);
    return;
    auto result = WaitForSingleObject(sleepMutex, 0);
    if(result == WAIT_TIMEOUT)
    {
        return;
    }
    lockMutex();

}

void sleepReleaseMutex()
{
    SetEvent(sleepEvent);
    return;
    releaseMutex();
    ReleaseMutex(sleepMutex);
}

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

void killUnwantedThreads()
{
    DWORD base = *reinterpret_cast<LPDWORD>(0x10BF10A4);
    DWORD threadEntry = base + 0x4A7B8;
    auto threads = suspendThreads();
    auto ntDll = LoadLibraryW(L"C:\\l2data\\ntdllc.dll");
    FQit ntQit = reinterpret_cast<FQit>(GetProcAddress(ntDll, "NtQueryInformationThread"));
    FZwOt zwOt = reinterpret_cast<FZwOt>(GetProcAddress(ntDll, "ZwOpenThread"));
    if(ntQit == NULL)
    {
        outputDebug("cannot load NtQueryInformationThread");
        return;
    }
    for(auto thread : *threads)
    {
        HANDLE th = reinterpret_cast<HANDLE>(0x10000);
        SomeShit1 ss1;
        SomeShit2 ss2;
        ss2.threadId = thread;
        zwOt(&th, THREAD_ALL_ACCESS, &ss1, &ss2);
        if(th == NULL)
        {
            outputDebug("failed to open thread");
        }
        else
        {
            std::stringstream str;
            str << th;
            outputDebug(str.str().c_str());
        }
        if(th != NULL)
        {
            DWORD entry;
            DWORD arg = 0x09;
            auto status = ntQit(th, arg, &entry, sizeof(entry), NULL);

            std::stringstream str;
            if(NT_SUCCESS(status))
            {

                /*wstr << "thread id = " << (LPVOID)thread.th32ThreadID << ", entry = " <<
                        (LPVOID)entry << ", arg = " << arg << ", handle = " << th;*/
                if(entry == threadEntry)
                {
                    TerminateThread(th, 1);
                }
                ResumeThread(th);
            }
            else
            {
                str << "query fail";
            }
            outputDebug(str.str().data());
            CloseHandle(th);
        }
    }
}
