#include "abr.h"
#include "dllhacklibrary.h"
#include "lineageipc.h"
#include "lineage/lineageglobal.h"
#include "utils.h"
#include "ipc.h"
#include "synchronization.h"
#include <ctime>
#include <fstream>
#include "patches.h"

#pragma comment(lib, "Kernel32.lib")

using namespace dhl;

static char buffer[256];
static HANDLE sharedMemoryHandle;
static BYTE* sharedMemoryData;


DLL_DEFAULT_ENTRY(entry);


void entry()
{
    LineageGlobal::instance();
    srand(time(NULL));
    initSynchronization();
    applyPatches();
    initPipes();
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
    case l2ipc::Command::USE_SKILL:
        l2ipc::sendReply(pipe, useSkill(*reinterpret_cast<LPDWORD>(message + sizeof(l2ipc::Command))));
        break;
    case l2ipc::Command::ASSIST:
        assist();
        l2ipc::sendReply(pipe, true);
        break;
    case l2ipc::Command::MOVE_TO:
        l2ipc::sendReply(pipe, moveToCommand(reinterpret_cast<Point3F*>(message + sizeof(l2ipc::Command))));
        break;
    case l2ipc::Command::NPC_CHAT:
        l2ipc::sendReply(pipe, npcChatCommand(*reinterpret_cast<LPDWORD>(message + sizeof(l2ipc::Command))));
        break;
    case l2ipc::Command::ACCEPT_ACTION:
        l2->acceptAction();
        l2ipc::sendReply(pipe, true);
        break;
    case l2ipc::Command::SPEAK_TO:
        l2ipc::sendReply(pipe, speakToCommand(*reinterpret_cast<LPDWORD>(message + sizeof(l2ipc::Command))));
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
    while(l2->isDroppedItemPresent(item.address()))
    {
        Sleep(100);
        if(getMilliSpan(actionTime) > 500)
        {
            l2->performActionOn(item.getID());
            actionTime = getMilliCount();
        }
        if(getMilliSpan(startTime) > 1600)
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

bool useSkill(DWORD id)
{
    auto player = LineageGlobal::instance()->getPlayer();
    if(player.isCasting())
    {
        return false;
    }

    auto l2 = LineageGlobal::instance();
    auto startTime = getMilliCount();
    auto actionTime = getMilliCount();
    l2->useSkill(id);

    while(!player.isCasting())
    {
        Sleep(100);
        if(getMilliSpan(actionTime) > 300)
        {
            l2->useSkill(id);
            actionTime = getMilliCount();
        }
        if(getMilliSpan(startTime) > 500)
        {
            return false;
        }
    }
    while(player.isCasting())
        Sleep(20);
    return true;
}

void assist()
{
    LineageGlobal::instance()->assist();
}

bool moveToCommand(Point3F *message)
{
    LineageGlobal::instance()->moveTo(message->x, message->y, message->z);
    return true;
}

void detach()
{
    auto dll = GetModuleHandle(L"abr.dll");
    if(dll != NULL)
    {
        removePatches();
        FreeLibraryAndExitThread(dll, 1);
    }
}

bool speakToCommand(DWORD npcId)
{
    auto l2 = LineageGlobal::instance();
    auto mobs = l2->getMobs();
    Mob speakToMob(0);
    for(auto mob : mobs)
    {
        if(mob.isNPC() && mob.getMobTypeID() == npcId)
        {
            speakToMob.setAddress(mob.address());
            break;
        }
    }

    if(speakToMob.address() == 0)
        return false;

    auto listAddress = l2->getNpcChatList();
    auto listId = *reinterpret_cast<LPDWORD>(listAddress + 0x28C);
    l2->focusMob(speakToMob.address());
    Sleep(100);
    l2->attack();
    auto time = getMilliCount();
    while(*reinterpret_cast<LPDWORD>(listAddress + 0x28C) == listId)
    {
        Sleep(500);
        l2->attack();
        if(getMilliSpan(time) > 5000)
            return false;
    }
    return true;
}

bool npcChatCommand(DWORD index)
{
    auto l2 = LineageGlobal::instance();
    auto listAddress = l2->getNpcChatList();
    auto listId = *reinterpret_cast<LPDWORD>(listAddress + 0x28C);
    l2->npcChat(index);
    auto time = getMilliCount();
    while(*reinterpret_cast<LPDWORD>(listAddress + 0x28C) == listId)
    {
        Sleep(500);
        if(getMilliSpan(time) > 1500)
            return false;
    }
    return true;
}
