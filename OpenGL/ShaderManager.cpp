#include "ShaderManager.h"
#include "Shader.h"
#include "Utils.h"

#include <QDebug>
#include <QDirIterator>
#include <QStringList>

#include <maya/MGlobal.h>
#include <maya/MQtUtil.h>

namespace mush3d{

ShaderManager::ShaderManager()
{

}

ShaderManager::~ShaderManager()
{
    for(int i=0; i<_shaders.size(); i++)
    {
        delete _shaders[i];
    }
}

ShaderManager * ShaderManager::getInstance()
{
    static ShaderManager g;
    return & g;
}

void ShaderManager::initialize()
{
    // read shaders files to strings
    QMap<QString, QString> shaderFiles;

    QDirIterator it(":", QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        QString currentFile = it.next();

        if(currentFile.contains(".glsl"))
        {

            QStringList parts =currentFile.split("/");

            if(parts.size() > 0)
            {
                QString n = parts[parts.size()-1];

                if(n.contains(".glsl"))
                {
                    QStringList np = n.split(".glsl");
                    shaderFiles[np[0]] = readFile(currentFile);
                }
            }
        }
    }

    // include types or functions
    foreach ( const auto & key, shaderFiles.keys() )
    {
        if(key == "include_types" || key == "functions")
            continue;

        QString rawCode = shaderFiles[key];
        QStringList rawLines = rawCode.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);

        QString code;

        for(int i=0; i<rawLines.size(); i++)
        {
            QString l = rawLines.at(i);

            if(l.contains("#include") == true && l.contains("//") == false)
            {
                QStringList includeCommand = l.split(QRegExp(" "));
                int num_parts = includeCommand.size();

                if(num_parts > 1)
                {
                    QString file_name = "include_" + includeCommand[1];

                    QString include_file = shaderFiles[file_name];

                    // find replace commands
                    for(int i=0; i<num_parts; i++)
                    {
                        if(includeCommand[i].contains("replace"))
                        {
                            QStringList replace_command = includeCommand[i].split("_");

                            if(replace_command.size() == 3)
                            {
                                QString from = replace_command[1];
                                QString to = replace_command[2];

                                include_file.replace(from, to);
                            }
                        }
                    }

                    code += "\n";
                    code += include_file;
                }
            }
            else
            {
                code += "\n";
                code += l;
            }
        }

        shaderFiles[key] = code;
    }

    // MatCap
    _shaders.push_back(new Shader(Shader::kMatcap));
    _shaders.back()->addCode(QOpenGLShader::Vertex,   shaderFiles["matcap_vertex"]);
    _shaders.back()->addCode(QOpenGLShader::Geometry, shaderFiles["matcap_geometry"]);
    _shaders.back()->addCode(QOpenGLShader::Fragment, shaderFiles["matcap_fragment"]);
    _shaders.back()->build();

    // Phong
    _shaders.push_back(new Shader(Shader::kPhong));
    _shaders.back()->addCode(QOpenGLShader::Vertex,   shaderFiles["phong_vertex"]);
    _shaders.back()->addCode(QOpenGLShader::Geometry, shaderFiles["phong_geometry"]);
    _shaders.back()->addCode(QOpenGLShader::Fragment, shaderFiles["phong_fragment"]);
    _shaders.back()->link();

    // Deferred
    _shaders.push_back(new Shader(Shader::kDeferred));
    _shaders.back()->addCode(QOpenGLShader::Vertex,   shaderFiles["deffered_vertex"]);
    _shaders.back()->addCode(QOpenGLShader::Geometry, shaderFiles["deferred_geometry"]);
    _shaders.back()->addCode(QOpenGLShader::Fragment, shaderFiles["deferred_fragment"]);
    _shaders.back()->link();

    // normals
    _shaders.push_back(new Shader(Shader::kNormals));
    _shaders.back()->addCode(QOpenGLShader::Vertex,   shaderFiles["normals_vertex"]);
    _shaders.back()->addCode(QOpenGLShader::Fragment, shaderFiles["normals_fragment"]);
    _shaders.back()->link();

    // Weight
    _shaders.push_back(new Shader(Shader::kWeights));
    _shaders.back()->addCode(QOpenGLShader::Vertex,   shaderFiles["weights_vertex"]);
    _shaders.back()->addCode(QOpenGLShader::Geometry, shaderFiles["weights_geometry"]);
    _shaders.back()->addCode(QOpenGLShader::Fragment, shaderFiles["weights_fragment"]);
    _shaders.back()->link();

    // Points 2d
    _shaders.push_back(new Shader(Shader::kConstant));
    _shaders.back()->addCode(QOpenGLShader::Vertex,   shaderFiles["constant_vertex"]);
    _shaders.back()->addCode(QOpenGLShader::Fragment, shaderFiles["constant_fragment"]);
    _shaders.back()->link();

    _shaders.push_back(new Shader(Shader::kFaceNormals));
    _shaders.back()->addCode(QOpenGLShader::Compute,  shaderFiles["faceNormals"]);
    _shaders.back()->link();

    _shaders.push_back(new Shader(Shader::kIntersectRayMesh));
    _shaders.back()->addCode(QOpenGLShader::Compute,  shaderFiles["intersectRayMesh"]);
    _shaders.back()->link();

    _shaders.push_back(new Shader(Shader::kPicking));
    _shaders.back()->addCode(QOpenGLShader::Compute,  shaderFiles["picking"]);
    _shaders.back()->link();

    _shaders.push_back(new Shader(Shader::kPickingTopological));
    _shaders.back()->addCode(QOpenGLShader::Compute,  shaderFiles["pickingTopological"]);
    _shaders.back()->link();

    _shaders.push_back(new Shader(Shader::kArea));
    _shaders.back()->addCode(QOpenGLShader::Compute,  shaderFiles["area"]);
    _shaders.back()->link();

    _shaders.push_back(new Shader(Shader::kPropagateDeltas));
    _shaders.back()->addCode(QOpenGLShader::Compute,  shaderFiles["propagateDeltas"]);
    _shaders.back()->link();

    _shaders.push_back(new Shader(Shader::kaddDeltas));
    _shaders.back()->addCode(QOpenGLShader::Compute,  shaderFiles["addDeltas"]);
    _shaders.back()->link();

    _shaders.push_back(new Shader(Shader::kAddWeights));
    _shaders.back()->addCode(QOpenGLShader::Compute,  shaderFiles["addWeights"]);
    _shaders.back()->link();

    _shaders.push_back(new Shader(Shader::kClayBrush));
    _shaders.back()->addCode(QOpenGLShader::Compute,  shaderFiles["clayBrush"]);
    _shaders.back()->link();

    _shaders.push_back(new Shader(Shader::kMoveBrush));
    _shaders.back()->addCode(QOpenGLShader::Compute,  shaderFiles["moveBrush"]);
    _shaders.back()->link();

    _shaders.push_back(new Shader(Shader::kCloneBrush));
    _shaders.back()->addCode(QOpenGLShader::Compute,  shaderFiles["cloneBrush"]);
    _shaders.back()->link();

    _shaders.push_back(new Shader(Shader::kWrinkle));
    _shaders.back()->addCode(QOpenGLShader::Compute,  shaderFiles["wrinkle"]);
    _shaders.back()->link();

    _shaders.push_back(new Shader(Shader::kMaskBrush));
    _shaders.back()->addCode(QOpenGLShader::Compute,  shaderFiles["maskBrush"]);
    _shaders.back()->link();

    _shaders.push_back(new Shader(Shader::kStandardBrush));
    _shaders.back()->addCode(QOpenGLShader::Compute,  shaderFiles["standardBrush"]);
    _shaders.back()->link();

    _shaders.push_back(new Shader(Shader::kCreaseBrush));
    _shaders.back()->addCode(QOpenGLShader::Compute,  shaderFiles["creaseBrush"]);
    _shaders.back()->link();

    _shaders.push_back(new Shader(Shader::kInflateBrush));
    _shaders.back()->addCode(QOpenGLShader::Compute,  shaderFiles["inflateBrush"]);
    _shaders.back()->link();

    _shaders.push_back(new Shader(Shader::kEraseBrush));
    _shaders.back()->addCode(QOpenGLShader::Compute,  shaderFiles["eraseBrush"]);
    _shaders.back()->link();

    _shaders.push_back(new Shader(Shader::kClothBrush));
    _shaders.back()->addCode(QOpenGLShader::Compute,  shaderFiles["clothBrush"]);
    _shaders.back()->link();

    _shaders.push_back(new Shader(Shader::kExtractDeltas));
    _shaders.back()->addCode(QOpenGLShader::Compute,  shaderFiles["extractDeltas"]);
    _shaders.back()->link();

    _shaders.push_back(new Shader(Shader::kRelaxBrush));
    _shaders.back()->addCode(QOpenGLShader::Compute,  shaderFiles["relaxBrush"]);
    _shaders.back()->link();

    _shaders.push_back(new Shader(Shader::kTransformBrush));
    _shaders.back()->addCode(QOpenGLShader::Compute,  shaderFiles["transformBrush"]);
    _shaders.back()->link();

    _shaders.push_back(new Shader(Shader::kWeightsBrush));
    _shaders.back()->addCode(QOpenGLShader::Compute,  shaderFiles["weightsBrush"]);
    _shaders.back()->link();

    _shaders.push_back(new Shader(Shader::kRelaxDeformer));
    _shaders.back()->addCode(QOpenGLShader::Compute,  shaderFiles["relaxDeformer"]);
    _shaders.back()->link();

    _shaders.push_back(new Shader(Shader::kDummyDeformer));
    _shaders.back()->addCode(QOpenGLShader::Compute,  shaderFiles["dummyDeformer"]);
    _shaders.back()->link();

    _shaders.push_back(new Shader(Shader::kMushDeformer));
    _shaders.back()->addCode(QOpenGLShader::Compute,  shaderFiles["mushDeformer"]);
    _shaders.back()->link();

    _shaders.push_back(new Shader(Shader::kCloneDeformer));
    _shaders.back()->addCode(QOpenGLShader::Compute,  shaderFiles["cloneDeformer"]);
    _shaders.back()->link();

    _shaders.push_back(new Shader(Shader::kBlendPoints));
    _shaders.back()->addCode(QOpenGLShader::Compute,  shaderFiles["blendPoints"]);
    _shaders.back()->link();

    _shaders.push_back(new Shader(Shader::kMuscleJiggle));
    _shaders.back()->addCode(QOpenGLShader::Compute,  shaderFiles["muscleJiggle"]);
    _shaders.back()->link();

    _shaders.push_back(new Shader(Shader::kVertexNormals));
    _shaders.back()->addCode(QOpenGLShader::Compute,  shaderFiles["vertexNormals"]);
    _shaders.back()->link();

    _shaders.push_back(new Shader(Shader::kJiggleDeformer));
    _shaders.back()->addCode(QOpenGLShader::Compute,  shaderFiles["jiggleDeformer"]);
    _shaders.back()->link();

    _shaders.push_back(new Shader(Shader::kTransform));
    _shaders.back()->addCode(QOpenGLShader::Compute,  shaderFiles["transform"]);
    _shaders.back()->link();

}

Shader * ShaderManager::getShader(Shader::ShaderType t)
{
    for(int i=0; i<_shaders.size(); i++)
    {
        if(_shaders[i]->getType() == t)
        {
            return _shaders[i];
        }
    }

    return nullptr;
}


QString ShaderManager::readFile(const QString & path)
{
    QFile f(path);
    QString header;
    if (f.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream in(&f);
        return in.readAll();
    }
    return QString();
}

}
