#include "MaskBrush.h"
#include "Camera.h"
#include "Global.h"
#include "GPU.h"
#include "ShaderManager.h"
#include "Mesh.h"
#include "SubdivisionLevel.h"

namespace mush3d{

MaskBrush::MaskBrush() : Brush()
{

}

MaskBrush::~MaskBrush()
{

}

void MaskBrush::press(const QPoint & mouse)
{
    Brush::press(mouse);

}

void MaskBrush::drag(const QPoint & mouse)
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

        // save brush matrix for drawing
        Brush::matrix = meshPoint.tangentSpace;

        // set falloff
        if(Brush::topological)
        {
            GPU::getInstance()->pickingTopological(_mesh, Brush::radius3d);
        }
        else
        {
            GPU::getInstance()->picking(_mesh, Brush::radius3d);
        }

        // sculpt
        Shader * shader = ShaderManager::getInstance()->getShader(Shader::kMaskBrush);

        int numVertices = _level->getNumVertices();

        _level->mask()->bind(0);
        _level->falloff1()->bind(1);
        _level->vertexRings()->bind(2);
        _level->vertexRingsSC()->bind(3);

        GLFunctions * gl = GLFunctions::getInstance();

        shader->bind();

        shader->setUniformValue("Intensity", Brush::intensity * Brush::pressure);
        shader->setUniformValue("Control", Brush::control);
        shader->setUniformValue("Shift", Brush::shift);
        shader->setUniformValue("NumVertices", numVertices);

        int numWorkGroups = numVertices / WORKGROUPSIZE + 1;
        gl->glDispatchCompute(numWorkGroups, 1, 1);
        gl->glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

        shader->release();
    }

}

void MaskBrush::release()
{
    if(canSculpt() == false) return;

    Brush::release();

}

ToolType MaskBrush::getType()
{
    return ToolType::kMaskingTool;
}

}
