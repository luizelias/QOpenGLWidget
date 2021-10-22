#ifndef UTILS_H
#define UTILS_H

#include "Types.h"

namespace mush3d{

namespace Utils
{
    static unsigned int INVALID_ID = 429496729;

    void Bresenham(int x1, int y1, int const x2, int const y2, QVector<QPoint> & pixels);

};

}

#endif // UTILS_H
