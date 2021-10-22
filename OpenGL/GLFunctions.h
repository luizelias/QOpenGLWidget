#ifndef GLFUNCTIONS_H
#define GLFUNCTIONS_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions_4_3_Core>

namespace mush3d{


class GLFunctions : public QOpenGLFunctions_4_3_Core
{
private:
    GLFunctions(){}

public:
    ~GLFunctions(){}

    // remove copy and assign operator
    GLFunctions(const GLFunctions&) = delete;
    void operator=(GLFunctions const&) = delete;

    static GLFunctions * getInstance();

    void initialize();
    void check();

    bool isInitialized(){return _initialized;}

private:
    bool _initialized = false;

};

}

#endif // GLFUNCTIONS_H
