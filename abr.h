#ifndef ABR_H
#define ABR_H

#include <Windows.h>
#include "utils.h"

void dispatchCommandMessage(HANDLE pipe, BYTE *message);
void dispatchDataMessage(HANDLE pipe, BYTE *message);

bool pickUpItem(DWORD itemAddress);
bool actionOnMob(DWORD mobAddress);
bool useSkill(DWORD id);
void performActionOnCommand(HANDLE pipe, LPBYTE message);
bool moveToCommand(Point3F *message);
void assist();
bool speakToCommand(DWORD npcId);
bool npcChatCommand(DWORD index);

void detach();



#endif // ABR_H
