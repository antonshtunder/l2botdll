#ifndef ABR_H
#define ABR_H

#include <Windows.h>

void dispatchCommandMessage(BYTE *message);
void dispatchDataMessage(HANDLE pipe, BYTE *message);

#endif // ABR_H
