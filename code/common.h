#ifndef COMMON_H
#define COMMON_H

#define NBIT 32


enum Plane {XY, YZ, ZX};

inline Plane intToPlane(const int &norm)
{
    if(norm == 0) return YZ;
    if(norm == 1) return ZX;
    return XY;
}

#endif // COMMON_H
