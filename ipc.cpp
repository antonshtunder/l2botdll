#include "abr.h"
#include "ipc.h"
#include "dllhacklibrary.h"
#include <sstream>
#include "winfuncs.h"
using namespace dhl;

static DWORD commandThread;
static DWORD dataThread;

static HANDLE pipeMutex;

DWORD CALLBACK commandPipeLoop(LPVOID pipe)
{
    WaitForSingleObject(pipeMutex, INFINITE);
    BYTE buffer[256];
    while(true)
    {
        if(ReadFile(pipe, buffer, sizeof(buffer), NULL, NULL))
        {
            DWORD command = reinterpret_cast<LPDWORD>(buffer)[0];
            if(command != l2ipc::Command::TEST)
            {
                outputDebug("dispatching command");
                outputDebug(numToStr(command, 10));
            }
            dispatchCommandMessage(pipe, buffer);
            if(command != l2ipc::Command::TEST)
            {
                outputDebug("command complete");
            }
        }
        else
        {
            //alert("fail");
            //detach();
            ReleaseMutex(pipeMutex);
            return 1;
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
            WaitForSingleObject(pipeMutex, INFINITE);
            CloseHandle(pipeMutex);
            detach();
            return 1;
        }
    }
}
void initPipes()
{
    pipeMutex = CreateMutex(NULL, FALSE, NULL);
    auto pipe = l2ipc::connectToCommandPipe();
    auto thread = CreateThread(NULL, 0, commandPipeLoop, pipe, 0, &commandThread);
    CloseHandle(thread);

    pipe = l2ipc::connectToDataManagmentPipe();
    thread = CreateThread(NULL, 0, dataPipeLoop, pipe, 0, &dataThread);
    CloseHandle(thread);
}
