#ifndef ABR_H
#define ABR_H

#include <Windows.h>

void dispatchCommandMessage(HANDLE pipe, BYTE *message);
void dispatchDataMessage(HANDLE pipe, BYTE *message);

bool pickUpItem(DWORD itemAddress);
bool actionOnMob(DWORD mobAddress);
void performActionOnCommand(HANDLE pipe, LPBYTE message);

#endif // ABR_H
