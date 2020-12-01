#ifndef COMMON_H
#define COMMON_H

#include <chrono>

#define NBIT 32


enum Plane {XY, YZ, ZX};

inline Plane intToPlane(const int &norm)
{
    if(norm == 0) return YZ;
    if(norm == 1) return ZX;
    return XY;
}

inline static std::chrono::time_point<std::chrono::system_clock> startChrono()
{
    return std::chrono::system_clock::now();
}

inline double stopChrono(std::chrono::time_point<std::chrono::system_clock> &start)
{
    auto time = std::chrono::system_clock::now() - start;
    return std::chrono::duration <double, std::milli> (time).count() / 1000;
}

#endif // COMMON_H
