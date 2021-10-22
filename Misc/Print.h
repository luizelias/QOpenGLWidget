#ifndef PRINT_H
#define PRINT_H

#include "Types.h"
#include <maya/MGlobal.h>
#include <maya/MString.h>

namespace mush3d
{
class Print
{
public:
    static void print(const vec4Array & x);
    static void print(const uintArray & x);
};

}

#endif // PRINT_H
