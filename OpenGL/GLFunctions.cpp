#include "GLFunctions.h"
#include <maya/MGlobal.h>

namespace mush3d{

GLFunctions * GLFunctions::getInstance()
{
    static GLFunctions g;

    if(g.isInitialized() == false)
    {
        g.initialize();
    }

    return & g;
}

void GLFunctions::initialize()
{
    if(_initialized) return;

    MGlobal::displayInfo("initializing opengl functions for mush 3d");

    if(!initializeOpenGLFunctions())
    {
        MGlobal::displayError("error initializing opengl");
        return;
    }

    _initialized = true;
}

void GLFunctions::check()
{
    GLenum error = GL_NO_ERROR;
    do {

        error = glGetError();

        if (error != GL_NO_ERROR)
        {
            if(error == GL_INVALID_ENUM) MGlobal::displayError("GL_INVALID_ENUM");
            else if(error == GL_INVALID_VALUE) MGlobal::displayError("GL_INVALID_VALUE");
            else if(error == GL_INVALID_OPERATION) MGlobal::displayError("GL_INVALID_OPERATION");
            else if(error == GL_STACK_OVERFLOW) MGlobal::displayError("GL_STACK_OVERFLOW");
            else if(error == GL_STACK_UNDERFLOW) MGlobal::displayError("GL_STACK_UNDERFLOW");
            else if(error == GL_OUT_OF_MEMORY) MGlobal::displayError("GL_OUT_OF_MEMORY");
            else if(error == GL_TABLE_TOO_LARGE) MGlobal::displayError("GL_TABLE_TOO_LARGE");
            else if(error == GL_CONTEXT_LOST) MGlobal::displayError("GL_CONTEXT_LOST");
        }

    } while (error != GL_NO_ERROR);
}

}
