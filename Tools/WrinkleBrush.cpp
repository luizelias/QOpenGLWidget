#include "WrinkleBrush.h"
#include "Camera.h"
#include "Global.h"
#include "GPU.h"
#include "ShaderManager.h"
#include "Mesh.h"
#include "SubdivisionLevel.h"

namespace mush3d{

WrinkleBrush::WrinkleBrush() : Brush()
{

}

WrinkleBrush::~WrinkleBrush()
{

}

void WrinkleBrush::press(const QPoint & mouse)
{
    Brush::press(mouse);

    // initialize
    if(canSculpt() == false) return;

    // init particles
    GPU::getInstance()->initWrinkles(_mesh, true);
}

void WrinkleBrush::drag(const QPoint & mouse)
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

        // apply wrinkles
        float envelope = Brush::intensity * Brush::pressure;

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

}

void WrinkleBrush::release()
{
    if(canSculpt() == false) return;

    Brush::release();
}

ToolType WrinkleBrush::getType()
{
    return ToolType::kWrinkleTool;
}

}
