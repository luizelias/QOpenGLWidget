#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H


#include "Shader.h"

#include <QMap>
#include <QString>

namespace mush3d{

class ShaderManager
{
private:
    ShaderManager();

public:
    virtual ~ShaderManager();

    // remove copy and assign operator
    ShaderManager(const ShaderManager&) = delete;
    void operator=(ShaderManager const&) = delete;

    static ShaderManager * getInstance();

    void initialize();

    Shader * getShader(Shader::ShaderType t);

private:
    QString readFile(const QString & path);

private:
    QVector<Shader*> _shaders;

};

}

#endif // SHADERMANAGER_H
