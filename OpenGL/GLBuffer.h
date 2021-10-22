#ifndef GLBUFFER_H
#define GLBUFFER_H

#include <QOpenGLFunctions>

#include <QDebug>
#include <QString>
#include <QVector>
#include <QObject>

#include "Types.h"
#include "Global.h"
#include "GLFunctions.h"

#include <maya/MGlobal.h>

namespace mush3d{

class GLBuffer
{
public:
    GLBuffer();
    ~GLBuffer();

    // Delete the copy constructor/assignment.
    GLBuffer(const GLBuffer &) = delete;
    GLBuffer &operator=(const GLBuffer &) = delete;

    // bind
    void bind(GLuint index);

    // unbind
    void unbind();

    // set data
    template <typename T> void setData(const QVector<T> &arr, GLuint index) {

        // get pointer to gl functions
        auto gl = GLFunctions::getInstance();

        // check if it's initialized
        if(_id == 0)
        {
            gl->glGenBuffers(1, &_id);
        }

        auto n = arr.size();

        if (n == 0)
            return;

        _numElements = n;
        _lastIndex = index;

        // size to upload
        size_t newSize = _numElements * sizeof(T);

        // bind the buffer
        gl->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, _id);

        // upload data
        if(newSize != _size)
        {
            // rebuild buffer
            gl->glBufferData(GL_SHADER_STORAGE_BUFFER, newSize, (void *)arr.data(), GL_DYNAMIC_COPY);
        }
        else
        {
            gl->glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, newSize, (void *)arr.data());
        }

        _size = newSize;

        // MEMORY BARRIER HERE???
        gl->glMemoryBarrier( GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT );
    }

    template <typename T> void getData(QVector<T> & arr, GLuint index)
    {
        if(_id == 0 || _size == 0)
            return;

        auto gl = GLFunctions::getInstance();

        // check if the type matches
        if((sizeof(T) * _numElements) != _size)
        {
            MGlobal::displayError("can't download buffer from GPU, wrong type");
            return;
        }

        arr.resize(_numElements);

        gl->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, _id);
        gl->glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, index, _size, (void *)arr.data());
    }


    GLuint id(){return _id;}
    size_t size(){return _size;}
    int numElements(){return _numElements;}

    void copyFrom(GLBuffer * other);

private:
    GLuint _id = 0;
    size_t _size = 0;
    int _numElements = 0;
    GLuint _lastIndex = 0;
};



}

#endif // GLBUFFER_H
