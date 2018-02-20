#ifndef UTILS_H
#define UTILS_H

#include <Windows.h>

typedef unsigned short* u16str;

struct Point3F
{
    float x;
    float y;
    float z;
};

float getDistance(const Point3F *loc1, const Point3F *loc2);

int getMilliCount();

int getMilliSpan(int nTimeStart);

int getRandom(int min, int max);


#endif // UTILS_H
