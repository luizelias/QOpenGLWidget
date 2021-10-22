#ifndef SHADER_H
#define SHADER_H

#include <QOpenGLShaderProgram>

namespace mush3d{

class Shader : public QOpenGLShaderProgram
{
public:
    enum ShaderType{kClayBrush,
                    kMoveBrush,
                    kStandardBrush,
                    kCreaseBrush,
                    kInflateBrush,
                    kEraseBrush,
                    kCloneBrush,
                    kMaskBrush,
                    kClothBrush,
                    kRelaxBrush,
                    kTransformBrush,
                    kWeightsBrush,

                    kExtractDeltas,

                    kDummyDeformer,

                    kMushDeformer,

                    kCloneDeformer,

                    kBlendPoints,

                    kMuscleJiggle,

                    kJiggleDeformer,

                     kTransform,


                    kFaceNormals, kVertexNormals, kPhong, kDeferred, kConstant, kWeights, kIntersectRayMesh, kPicking, kPickingTopological,
                    kArea, kLocalToWorld, kPropagateDeltas, kaddDeltas, kAddWeights, kMatcap, kNormals, kAverageDeltas,kWrinkle,kRelaxDeformer
                   };

public:
    Shader(ShaderType t, QObject * parent = nullptr);

    void addCode(QOpenGLShader::ShaderTypeBit type, const QString & code);
    void build();

    ShaderType getType();

private:
    ShaderType type;
};

}

#endif // SHADER_H


