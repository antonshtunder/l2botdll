#include "abr.h"
#include "dllhacklibrary.h"
#include "lineageipc.h"
#include "lineage/lineageglobal.h"
#include <cmath>
#include <ctime>

#pragma comment(lib, "Kernel32.lib")

using namespace dhl;

char buffer[256];
static HANDLE sharedMemoryHandle;
static BYTE* sharedMemoryData;

DLL_DEFAULT_ENTRY(entry);

DWORD CALLBACK commandPipeLoop(LPVOID pipe)
{
    BYTE buffer[256];
    while(true)
    {
        if(ReadFile(pipe, buffer, sizeof(buffer), NULL, NULL))
        {
            dispatchCommandMessage(buffer);
        }
        else
        {
            //alert("fail");
        }
    }
}

DWORD CALLBACK dataPipeLoop(LPVOID pipe)
{
    BYTE buffer[256];
    while(true)
    {
        if(ReadFile(pipe, buffer, sizeof(buffer), NULL, NULL))
        {
            dispatchDataMessage(pipe, buffer);
        }
        else
        {
            //alert("fail");
        }
    }
}

void entry()
{
    srand(time(NULL));

    auto pipe = l2ipc::connectToCommandPipe();
    if(pipe == INVALID_HANDLE_VALUE)
    {
        alert(itoa(GetLastError(), buffer, 10));
    }
    auto thread = CreateThread(NULL, 0, commandPipeLoop, pipe, 0, NULL);
    CloseHandle(thread);

    pipe = l2ipc::connectToDataManagmentPipe();
    thread = CreateThread(NULL, 0, dataPipeLoop, pipe, 0, NULL);
    CloseHandle(thread);
}


void dispatchCommandMessage(BYTE *message)
{
    DWORD command = *reinterpret_cast<LPDWORD>(message);
    switch(command)
    {
    case l2ipc::Command::ATTACK:
        alert("attack");
        break;
    }
}

void dispatchDataMessage(HANDLE pipe, BYTE *message)
{
    DWORD command = *reinterpret_cast<LPDWORD>(message);
    l2ipc::DataCommand answer;
    auto l2g = LineageGlobal::instance();
    switch(command)
    {
    case l2ipc::DataCommand::INIT_MEMORY:
        sharedMemoryHandle = l2ipc::createSharedMemory(GetCurrentProcessId());
        sharedMemoryData = l2ipc::getSharedMemory(sharedMemoryHandle);
        answer = l2ipc::DataCommand::DATA_READY;
        WriteFile(pipe, &answer, 4, NULL, NULL);
        break;
    case l2ipc::DataCommand::GET_DATA:
        if(l2g->isInGame())
        {
            l2g->getRepresentation().toData(sharedMemoryData);
            answer = l2ipc::DataCommand::DATA_READY;
            WriteFile(pipe, &answer, 4, NULL, NULL);
        }
        else
        {
            answer = l2ipc::DataCommand::NOT_IN_GAME;
            WriteFile(pipe, &answer, 4, NULL, NULL);
        }
        break;
    }
}
