#include "abr.h"
#include "ipc.h"

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
void initPipes()
{
    auto pipe = l2ipc::connectToCommandPipe();
    auto thread = CreateThread(NULL, 0, commandPipeLoop, pipe, 0, NULL);
    CloseHandle(thread);

    pipe = l2ipc::connectToDataManagmentPipe();
    thread = CreateThread(NULL, 0, dataPipeLoop, pipe, 0, NULL);
    CloseHandle(thread);
}
