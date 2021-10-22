#include "CloneBrush.h"
#include "Camera.h"
#include "Global.h"
#include "GPU.h"
#include "Mesh.h"

namespace mush3d{


CloneBrush::CloneBrush() : Brush()
{

}

CloneBrush::~CloneBrush()
{

}

void CloneBrush::press(const QPoint & mouse)
{
    Brush::press(mouse);

    if(canSculpt() == false) return;

    // update morph points buffer
    vec4Array & points = Global::getInstance()->getMorphPoints();
    _mesh->morphTarget()->setData(points, 0);
}

void CloneBrush::drag(const QPoint & mouse)
{
    if(canSculpt() == false) return;

    // get pixels to sculpt
    Brush::drag(mouse);

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

        // sculpt
        GPU::getInstance()->cloneDetails(_mesh->morphTarget(),
                                    _mesh->positions(),
                                    _mesh->backupPositions(),
                                    _level->falloff1(),
                                    _level->vertexRings(),
                                    _level->vertexRingsSC(),
                                    _level->faceRings(),
                                    _level->faceRingsSC(),
                                    _level->faceIndices(),
                                    _mesh->basePoints(),
                                    _mesh->offsetPoints(),
                                    Brush::cloneIterations,
                                    Brush::intensity * Brush::pressure,
                                    true);

        // compute normals
        GPU::getInstance()->computeFaceNormals(_mesh->positions(), _level->faceIndices(), _level->faceNormals(), _level->getNumFaces());
    }

}

void CloneBrush::release()
{
    if(canSculpt() == false) return;

    Brush::release();

}

ToolType CloneBrush::getType()
{
    return ToolType::kCloneTool;
}

}
