#include "EraseBrush.h"
#include "Camera.h"
#include "Global.h"
#include "Global.h"
#include "GPU.h"
#include "ShaderManager.h"
#include "Mesh.h"

namespace mush3d{

EraseBrush::EraseBrush() : Brush()
{

}

EraseBrush::~EraseBrush()
{

}

void EraseBrush::press(const QPoint & mouse)
{
    Brush::press(mouse);

    if(canSculpt() == false) return;

    // copy base and offset points to buffers
    Mesh * mesh = Global::getInstance()->mesh();

    //mesh->basePoints()->setData(_sculptSample->getBasePoints());
    //mesh->offsetPoints()->setData(_sculptSample->getOffsetPoints());
}

void EraseBrush::drag(const QPoint & mouse)
{
    if(canSculpt() == false) return;

    // get pixels to sculpt
    Brush::drag(mouse);

    // sculpt
    for(QPoint & p: _pixels)
    {
        // get ray to world
        Ray ray = getRay(p);

        // hit mesh
        PointOnMesh meshPoint;

        if(GPU::getInstance()->intersectRayMesh(_mesh, ray, meshPoint) == false)
        {
            return;
        }

        // set falloff
        GPU::getInstance()->picking(_mesh, Brush::radius3d);

        // sculpt
        Shader * shader = ShaderManager::getInstance()->getShader(Shader::kEraseBrush);

        int numVertices = _level->getNumVertices();

        _mesh->positions()->bind(0);
        _level->falloff1()->bind(1);
        _level->brush()->bind(2);
        _mesh->basePoints()->bind(3);
        _mesh->offsetPoints()->bind(4);

        GLFunctions * gl = GLFunctions::getInstance();

        shader->bind();

        shader->setUniformValue("Intensity", Brush::intensity * Brush::pressure);
        shader->setUniformValue("NumVertices", _level->getNumVertices());

        int numWorkGroups = numVertices / WORKGROUPSIZE + 1;
        gl->glDispatchCompute(numWorkGroups, 1, 1);
        gl->glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

        shader->release();

        // compute normals
        GPU::getInstance()->computeFaceNormals(_mesh->positions(), _level->faceIndices(), _level->faceNormals(), _level->getNumFaces());
    }

}

void EraseBrush::release()
{
    if(canSculpt() == false) return;

    Brush::release();

}

ToolType EraseBrush::getType()
{
    return ToolType::kEraseTool;
}

}
