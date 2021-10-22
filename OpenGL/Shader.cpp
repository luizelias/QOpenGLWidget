#include "Shader.h"

#include <maya/MGlobal.h>
#include <maya/MQtUtil.h>

namespace mush3d{

Shader::Shader(ShaderType t, QObject * parent) : QOpenGLShaderProgram(parent), type(t)
{

}

Shader::ShaderType Shader::getType()
{
    return type;
}

void Shader::addCode(QOpenGLShader::ShaderTypeBit type, const QString & code)
{
    addShaderFromSourceCode(type, code);
    MGlobal::displayInfo(MQtUtil::toMString(log()));
}

void Shader::build()
{
    link();
    MGlobal::displayInfo(MQtUtil::toMString(log()));
}

}
