#include "ClothBrush.h"
#include "Camera.h"
#include "Global.h"
#include "GPU.h"
#include "ShaderManager.h"
#include "Mesh.h"

namespace mush3d{

ClothBrush::ClothBrush() : Brush()
{

}

ClothBrush::~ClothBrush()
{

}

void ClothBrush::press(const QPoint & mouse)
{
    Brush::press(mouse);

    if(canSculpt() == false) return;

    // save current position on backup
    _mesh->backupPositions()->copyFrom(_mesh->positions());

    // reset move delta
    Brush::moveDelta = vec3(0,0,0);

    // picking
    if(Brush::topological)
    {
        GPU::getInstance()->pickingTopological(_mesh, Brush::radius3d);
    }
    else
    {
        GPU::getInstance()->picking(_mesh, Brush::radius3d);
    }

    // set the rest pose for morph target
    _mesh->basePoints()->copyFrom(_mesh->morphTarget());
    _mesh->morphTarget()->copyFrom(_mesh->positions());
}

void ClothBrush::drag(const QPoint & mouse)
{
    if(canSculpt() == false) return;

    // project ray to plane
    Ray ray = getRay(mouse);

    // update move delta
    vec3 pp = _plane.intersect(ray);
    Brush::moveDelta = pp - _hitPoint;

    // get num vertices
    int numVertices = _level->getNumVertices();


    // sculpt
    Shader * shader = ShaderManager::getInstance()->getShader(Shader::kMoveBrush);

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

    // apply wrinkles
    float envelope = Brush::intensity * Brush::pressure;


    GPU::getInstance()->initWrinkles(_mesh, true);


    GPU::getInstance()->applyWrinkles(_mesh,
                                 envelope,
                                 Brush::wrinkleStructuralForce,
                                 Brush::wrinkleFlexionForce,
                                 Brush::wrinkleShearForce,
                                 Brush::wrinkleIterations,
                                 Brush::wrinkleDeltaTime,
                                 0.0f,
                                 true);

    // compute normals
    GPU::getInstance()->computeFaceNormals(_mesh->positions(), _level->faceIndices(), _level->faceNormals(), _level->getNumFaces());
}

void ClothBrush::release()
{
    if(canSculpt() == false) return;

    Brush::release();

    // restore morph target
    _mesh->morphTarget()->copyFrom(_mesh->basePoints());
}

ToolType ClothBrush::getType()
{
    return ToolType::kClothTool;
}

}
