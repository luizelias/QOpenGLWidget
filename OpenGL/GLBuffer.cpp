#include "GLBuffer.h"
#include "Global.h"
#include "Utils.h"
#include "GLFunctions.h"

namespace mush3d{

GLBuffer::GLBuffer() : _id(0)
{

}

GLBuffer::~GLBuffer()
{
    if(_id)
    {
        GLFunctions::getInstance()->glDeleteBuffers(1, & _id);
    }
}

void GLBuffer::copyFrom(GLBuffer * other)
{
    auto gl = GLFunctions::getInstance();

    // get size of source buffer
    auto sourceSize = other->size();

    gl->glBindBuffer(GL_SHADER_STORAGE_BUFFER, other->id());
    gl->glBindBuffer(GL_COPY_WRITE_BUFFER, _id);

    // resize before copy
    if(sourceSize != _size)
    {
        gl->glBufferData(GL_COPY_WRITE_BUFFER, sourceSize, NULL, GL_DYNAMIC_COPY);
    }

    gl->glCopyBufferSubData(GL_SHADER_STORAGE_BUFFER, GL_COPY_WRITE_BUFFER,0,0, _size);
    gl->glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    gl->glBindBuffer(GL_COPY_WRITE_BUFFER, 0);

    // MEMORY BARRIER HERE???
    gl->glMemoryBarrier( GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT );
}

// bind
void GLBuffer::bind(GLuint index)
{
    GLFunctions * gl = GLFunctions::getInstance();

    // check if it's initialized
    if(_id == 0)
    {
        gl->glGenBuffers(1, &_id);
    }

    gl->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, _id);
}

// unbind
void GLBuffer::unbind()
{
    GLFunctions * gl = GLFunctions::getInstance();

    // check if it's initialized
    if(_id == 0)
    {
        gl->glGenBuffers(1, &_id);
    }

    gl->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, _lastIndex, 0);

}

}


