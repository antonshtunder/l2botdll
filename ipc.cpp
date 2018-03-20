#include "abr.h"
#include "ipc.h"
#include "dllhacklibrary.h"
using namespace dhl;

static DWORD commandThread;
static DWORD dataThread;

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
            detach();
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
            //alert("fail");
            detach();
            return 1;
        }
    }
}
void initPipes()
{
    auto pipe = l2ipc::connectToCommandPipe();
    auto thread = CreateThread(NULL, 0, commandPipeLoop, pipe, 0, &commandThread);
    CloseHandle(thread);

    pipe = l2ipc::connectToDataManagmentPipe();
    thread = CreateThread(NULL, 0, dataPipeLoop, pipe, 0, &dataThread);
    CloseHandle(thread);
}
