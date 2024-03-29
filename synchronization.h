#ifndef THREADS_H
#define THREADS_H

void initSynchronization();
void lockMutex();
void sleepLockMutex();
void releaseMutex();
void sleepReleaseMutex();
void closeSynchronizationHandles();
void killUnwantedThreads();

#endif // THREADS_H
