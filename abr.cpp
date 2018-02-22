#include "abr.h"
#include "dllhacklibrary.h"
#include "lineageipc.h"
#include "lineage/lineageglobal.h"
#include "utils.h"
#include <ctime>

#pragma comment(lib, "Kernel32.lib")

using namespace dhl;

static char buffer[256];
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
            dispatchCommandMessage(pipe, buffer);
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


void dispatchCommandMessage(HANDLE pipe, BYTE *message)
{
    DWORD command = *reinterpret_cast<LPDWORD>(message);
    auto l2 = LineageGlobal::instance();
    switch(command)
    {
    case l2ipc::Command::DO_ACTION:
        l2->doAction(*reinterpret_cast<LPDWORD>(message + sizeof(l2ipc::Command)));
        l2ipc::sendReply(pipe, true);
        break;
    case l2ipc::Command::ATTACK:
        l2->attack();
        l2ipc::sendReply(pipe, true);
        break;
    case l2ipc::Command::PERFORM_ACTION_ON:
        performActionOnCommand(pipe, message + sizeof(l2ipc::Command));
        break;
    case l2ipc::Command::IS_MOB_DEAD:
        l2ipc::sendReply(pipe, Mob::isDead(*reinterpret_cast<LPDWORD>(message + sizeof(l2ipc::Command))));
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
        WriteFile(pipe, &answer, sizeof(answer), NULL, NULL);
        break;
    case l2ipc::DataCommand::GET_DATA:
        if(l2g->isInGame())
        {
            l2g->getRepresentation().toData(sharedMemoryData);
            answer = l2ipc::DataCommand::DATA_READY;
            WriteFile(pipe, &answer, sizeof(answer), NULL, NULL);
        }
        else
        {
            answer = l2ipc::DataCommand::NOT_IN_GAME;
            WriteFile(pipe, &answer, sizeof(answer), NULL, NULL);
        }
        break;
    }
}

bool pickUpItem(DWORD itemAddress)
{
    DroppedItem item(itemAddress);
    auto l2 = LineageGlobal::instance();

    auto startTime = getMilliCount();
    auto actionTime = getMilliCount();
    l2->performActionOn(item.getID());
    while(l2->isAddressInArray(itemAddress))
    {
        Sleep(150);
        if(getMilliSpan(actionTime) > 700)
        {
            l2->performActionOn(item.getID());
            actionTime = getMilliCount();
        }
        if(getMilliSpan(startTime) > 2000)
            return false;
    }
    return true;
}

void performActionOnCommand(HANDLE pipe, LPBYTE message)
{
    auto l2 = LineageGlobal::instance();
    DWORD instanceID = *reinterpret_cast<LPDWORD>(message);
    DWORD instanceAddress = *reinterpret_cast<LPDWORD>(message + sizeof(DWORD));
    Representations instanceType = *reinterpret_cast<Representations*>(message + sizeof(DWORD) * 2);
    if(instanceType == Representations::DROPPED_ITEM)
    {
        l2ipc::sendReply(pipe, pickUpItem(instanceAddress));
    }
    else if(instanceType == Representations::MOB)
    {
        l2ipc::sendReply(pipe, actionOnMob(instanceAddress));
    }
    else
    {
        l2->performActionOn(instanceID);
        l2ipc::sendReply(pipe, true);
    }
}

bool actionOnMob(DWORD mobAddress)
{
    if(!Mob::isFocused(mobAddress))
    {
        LineageGlobal::instance()->focusMob(mobAddress);
    }
    else
    {
        LineageGlobal::instance()->attack();
    }
    return true;
}
