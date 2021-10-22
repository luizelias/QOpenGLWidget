#include "MoveBrush.h"
#include "Global.h"
#include "GPU.h"
#include "ShaderManager.h"
#include "Mesh.h"
#include "SubdivisionLevel.h"

namespace mush3d{

MoveBrush::MoveBrush() : Brush()
{

}

MoveBrush::~MoveBrush()
{

}

void MoveBrush::press(const QPoint & mouse)
{
    Brush::press(mouse);

    if(canSculpt() == false) return;

    if(Brush::topological)
    {
        GPU::getInstance()->pickingTopological(_mesh, Brush::radius3d);
    }
    else
    {
        GPU::getInstance()->picking(_mesh, Brush::radius3d);
    }

    // save position to backup
    Global::getInstance()->mesh()->backupPositions()->copyFrom(_mesh->positions());

    // convert click normal to 2d
    vec3 p1 = _hitPoint;
    vec3 p2 = _hitPoint + _hitNormal;

    vec2 p1_2d = worldToView(p1);
    vec2 p2_2d = worldToView(p2);

    _pressNormal2D = (p2_2d - p1_2d).normalized();
    _pressPoint2D = p1_2d;
}

void MoveBrush::drag(const QPoint & mouse)
{
    if(canSculpt() == false) return;

    // project ray to plane

    // get current camera
    Ray ray = getRay(mouse);

    vec3 pp = _plane.intersect(ray);

    vec3 d = pp - _hitPoint;

    if(Brush::control)
    {
        // get the drag vector in 2d
        vec2 drag2D = (vec2(mouse.x(), mouse.y()) - _pressPoint2D).normalized();
        float dot = vec2::dotProduct(_pressNormal2D, drag2D);

        Brush::moveDelta = _hitNormal.normalized() * d.length() * dot;
    }
    else
    {
        Brush::moveDelta = d;
    }

    // sculpt
    Shader * shader = ShaderManager::getInstance()->getShader(Shader::kMoveBrush);

    int numVertices = _level->getNumVertices();

    _mesh->positions()->bind(0);
    _level->falloff1()->bind(1);
    _level->deltas()->bind(2);
    _level->mask()->bind(3);
    _level->brush()->bind(4);
    _mesh->backupPositions()->bind(5);
    _level->faceIndices()->bind(6);

    GLFunctions * gl = GLFunctions::getInstance();

    shader->bind();

    shader->setUniformValue("intensity", Brush::intensity);
    shader->setUniformValue("control", false);
    shader->setUniformValue("numVertices", numVertices);
    shader->setUniformValue("moveDelta", Brush::moveDelta);

    int numWorkGroups = numVertices / WORKGROUPSIZE + 1;
    gl->glDispatchCompute(numWorkGroups, 1, 1);
    gl->glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

    shader->release();

    // compute normals
    GPU::getInstance()->computeFaceNormals(_mesh->positions(), _level->faceIndices(), _level->faceNormals(), _level->getNumFaces());
}

void MoveBrush::release()
{
    if(canSculpt() == false) return;

    Brush::release();
}

ToolType MoveBrush::getType()
{
    return ToolType::kMoveTool;
}

}
