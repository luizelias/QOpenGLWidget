#include "Print.h"

namespace mush3d{

void Print::print(const vec4Array & x)
{
    int numElements = x.size();

    MString msg("vec4Array: \n");

    for(int i=0; i<numElements; i++)
    {
        msg += x[i].x(); msg += ", ";
        msg += x[i].y(); msg += ", ";
        msg += x[i].z(); msg += ", ";
        msg += x[i].w(); msg += "\n";
    }

    MGlobal::displayInfo(msg);
}

void Print::print(const uintArray & x)
{
    int numElements = x.size();

    MString msg("uintArray: ");

    for(int i=0; i<numElements; i++)
    {
        msg += x[i]; msg += ", ";
    }

    MGlobal::displayInfo(msg);
}

}
