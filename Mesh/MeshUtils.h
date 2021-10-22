#ifndef MESHUTILS_H
#define MESHUTILS_H

#include "Types.h"

#define ORIGINAL 0
#define FACE 1
#define EDGE 2

namespace mush3d{

class SubdivisionLevel;

namespace MeshUtils
{
    bool reconstructLevel(SubdivisionLevel * level,
                          int & numVerticesLow,
                          uintArray & faceVertices_low,
                          uintArray & faceCounts_low,
                          vec2Array & faceUvs_low);

}

}

#endif // MESHUTILS_H
