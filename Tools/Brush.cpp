#include "Brush.h"
#include "Global.h"
#include "Utils.h"
#include "Mesh.h"
#include "GPU.h"
#include "Global.h"
#include "Viewport.h"

namespace mush3d{

float Brush::pressure = 0.5f;
float Brush::intensity = 1.0f;
bool Brush::shift = false;
bool Brush::control = false;
int Brush::radius2d = 10;
float Brush::radius3d = 1.0f;
mat4 Brush::matrix = mat4();
vec3 Brush::moveDelta;
vec3 Brush::hitPoint;
bool Brush::topological = false;
int Brush::wrinkleIterations = 100;
float Brush::wrinkleStructuralForce = 0.8f;
float Brush::wrinkleFlexionForce = 0.8f;
float Brush::wrinkleShearForce = 0.8f;
float Brush::wrinkleDeltaTime = 0.2f;
int Brush::cloneIterations = 1;
int Brush::smoothIterations = 1;
int Brush::cloneSmoothBaseIterations = 10;

int Brush::max2dRadius = 500;

QOpenGLTexture * Brush::squareAlpha = nullptr;
QOpenGLTexture * Brush::dotAlpha = nullptr;

Brush::Brush() : Tool(), _level(nullptr)
{

}

Brush::~Brush()
{

}

void Brush::press(const QPoint & mouse)
{
    // save mouse press
    _mousePress = mouse;
    _mouseDrag = mouse;

    // reset
    _mesh = nullptr;
    _level = nullptr;

    _mesh = Global::getInstance()->mesh();
    if(!_mesh) return;

    // reset deltas on current level
    _level = _mesh->getCurrentLevel();

    if(!_level) return;

    _level->deltas()->setData(vec4Array(_level->getNumVertices(), vec4(0,0,0,0)), 0);

    // save press points for undo
    _mesh->positions()->getData(_pressPoints, 0);

    // reset brush buffer
    QVector<BrushData> bData(1);
    _level->brush()->setData(bData, 0);

    // Mouse Ray
    Ray ray = getRay(mouse);

    // hit mesh
    PointOnMesh meshPoint;
    if(GPU::getInstance()->intersectRayMesh(_mesh, ray, meshPoint) == false)
    {
        _mesh = nullptr;
        _level = nullptr;
        return;
    }

    auto n = ray.getNear();
    auto f = ray .getFar();

    MGlobal::displayInfo(MString("near=[") + n.x() +", " + n.y()+", "+n.z());
    MGlobal::displayInfo(MString("far=[") + f.x() +", " + f.y() +", " +f.z());



    //set camera point of interest
    _hitPoint = meshPoint.position;
    _hitNormal = meshPoint.normal;

    // calculate radius 3d
    setPointOfInterest(_hitPoint);

    _plane.setCenter(_hitPoint);
    _plane.setNormal((ray._far-ray._near).normalized());

    QPoint radiusPoint(mouse.x()+ Brush::radius2d, mouse.y());
    Ray ray_r = getRay(radiusPoint);

    Brush::radius3d = _hitPoint.distanceToPoint(_plane.intersect(ray_r));
}

void Brush::drag(const QPoint & mouse)
{
    if(canSculpt() == false) return;

    //mouse.manhattanLength()
    QVector<QPoint> allPixels;
    Utils::Bresenham(_mouseDrag.x(), _mouseDrag.y(), mouse.x(), mouse.y(), allPixels);

    // filter pixels by distance
    int distance = int((float)Brush::radius2d * 0.3f);

    if(distance > 30)
        distance = 30;

    int numPixels = allPixels.size();

    _pixels.clear();
    _pixels.reserve(numPixels);

    for(QPoint & p: allPixels)
    {
        QPoint d = p - _mouseDrag;
        if (d.manhattanLength() >= distance)
        {
            _pixels.append(p);
            _mouseDrag = p;
        }
    }
}

void Brush::release()
{
    if(canSculpt() == false) return;

    // get release points
    vec4Array releasePoints;
    _mesh->positions()->getData(releasePoints, 0);

    // make deltas
    int num_points = releasePoints.size();
    vec4Array sculpt_deltas(num_points);

    for(int i=0; i<num_points; i++)
    {
        sculpt_deltas[i] = releasePoints[i]-_pressPoints[i];
    }

    _level->deltas()->setData(sculpt_deltas, 0);

    // propagate deltas all the way up
    _mesh->propagateDeltas();

    // get deltas on top level after propagation
    SubdivisionLevel * topLevel = _mesh->topLevel();

    vec4Array deltas;
    topLevel->deltas()->getData(deltas, 0);

    // create command //
    //    SculptCommand * cmd = new SculptCommand(deltas, _sculptSample);
    //    cmd->doIt();

    //    Global::getUndoManager()->addCommand(cmd);

    Global::getInstance()->mainWindow()->viewport()->update();
}

bool Brush::canSculpt()
{
    if(!_level) return false;
    if(!_mesh) return false;

    return true;
}

}

