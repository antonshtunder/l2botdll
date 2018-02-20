#include "utils.h"
#include <cmath>
#include <ctime>
#include <sys/timeb.h>

float getDistance(const Point3F *loc1, const Point3F *loc2)
{
    return sqrtf(powf(loc1->x - loc2->x, 2.0f) + powf(loc1->y - loc2->y, 2.0f));
}

int getMilliCount(){
    timeb tb;
    ftime(&tb);
    int nCount = tb.millitm + (tb.time & 0xfffff) * 1000;
    return nCount;
}

int getMilliSpan(int nTimeStart){
    int nSpan = getMilliCount() - nTimeStart;
    if(nSpan < 0)
        nSpan += 0x100000 * 1000;
    return nSpan;
}

int getRandom(int min, int max)
{
    return min + (rand() % (max - min));
}
