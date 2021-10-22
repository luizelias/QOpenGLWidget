#include "StandardBrush.h"
#include "Camera.h"
#include "Global.h"
#include "GPU.h"
#include "ShaderManager.h"
#include "Mesh.h"
#include "SubdivisionLevel.h"

namespace mush3d{

StandardBrush::StandardBrush() : Brush()
{

}

StandardBrush::~StandardBrush()
{

}

void StandardBrush::press(const QPoint & mouse)
{
    Brush::press(mouse);

}

void StandardBrush::drag(const QPoint & mouse)
{
    if(canSculpt() == false) return;

    // get pixels to sculpt
    Brush::drag(mouse);

    // sculpt
    for(QPoint & p: _pixels)
    {
        // get ray to world
        Ray ray = getRay(p);

        float radius = Brush::radius3d;

        // scale 75 % of the radius by pressure and add the other 25%
        radius = (radius * 0.75 * Brush::pressure) + (radius * 0.25);

        // hit mesh
        PointOnMesh meshPoint;

        if(GPU::getInstance()->intersectRayMesh(_mesh, ray, meshPoint) == false)
        {
            return;
        }

        // save brush matrix for drawing
        Brush::matrix = meshPoint.tangentSpace;

        // set falloff
        GPU::getInstance()->picking(_mesh, Brush::radius3d);

        // compute area
        GPU::getInstance()->computeArea(_mesh->positions(), _level->faceNormals(), _level->falloff1(), _level->area(), _level->brush(), _level->faceRings(), _level->faceRingsSC(), _level->getNumVertices());

        // sculpt
        Shader * shader = ShaderManager::getInstance()->getShader(Shader::kStandardBrush);


            int numVertices = _level->getNumVertices();

            _mesh->positions()->bind(0);
            _level->falloff1()->bind(1);
            _level->deltas()->bind(2);
            _level->mask()->bind(3);
            _level->brush()->bind(4);
            _level->faceRings()->bind(5);
            _level->faceRingsSC()->bind(6);

            GLFunctions * gl = GLFunctions::getInstance();

            shader->bind();

            shader->setUniformValue("radius", radius);
            shader->setUniformValue("intensity", Brush::intensity);
            shader->setUniformValue("pressure", Brush::pressure);
            shader->setUniformValue("control", Brush::control);
            shader->setUniformValue("numVertices", numVertices);

            int numWorkGroups = numVertices / WORKGROUPSIZE + 1;
            gl->glDispatchCompute(numWorkGroups, 1, 1);
            gl->glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

            shader->release();



        // compute normals
        GPU::getInstance()->computeFaceNormals(_mesh->positions(), _level->faceIndices(), _level->faceNormals(), _level->getNumFaces());
    }

}

void StandardBrush::release()
{
    if(canSculpt() == false) return;

    Brush::release();

}

ToolType StandardBrush::getType()
{
    return ToolType::kStandardTool;
}

}
