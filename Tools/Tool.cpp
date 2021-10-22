#include "Tool.h"
#include "Global.h"
#include "MainWindow.h"
#include "Viewport.h"
#include "Camera.h"

#include <maya/MGlobal.h>

namespace mush3d{

int Tool::button = 0;

Tool::Tool() : _isSelected(false)
{}

Tool::~Tool(){}

void Tool::setSelected(bool s)
{
    _isSelected = s;
}

bool Tool::isSelected()
{
    return _isSelected;
}

Ray Tool::getRay(const QPoint & point)
{
    Camera * camera = Global::getInstance()->mainWindow()->viewport()->camera();

    vec3 nearP = camera->getNearPoint(point.x(), point.y());
    vec3 farP = camera->getFarPoint(point.x(), point.y());

    return Ray(nearP, farP);
}

void Tool::setPointOfInterest(const vec3 & point)
{
    Camera * camera = Global::getInstance()->mainWindow()->viewport()->camera();

    camera->setPointOfInterest(point);
}

float Tool::viewToWorld(int size, const vec3 referencePoint)
{
    Camera * camera = Global::getInstance()->mainWindow()->viewport()->camera();

    return camera->viewToWorld(size, referencePoint);
}

vec3 Tool::nearPoint(const QPoint &point)
{
    Camera * camera = Global::getInstance()->mainWindow()->viewport()->camera();

    return camera->getNearPoint(point.x(), point.y());
}

vec3 Tool::farPoint(const QPoint &point)
{
    Camera * camera = Global::getInstance()->mainWindow()->viewport()->camera();

    return camera->getFarPoint(point.x(), point.y());
}

vec2 Tool::worldToView(const vec3 &point)
{
    Camera * camera = Global::getInstance()->mainWindow()->viewport()->camera();

    return camera->worldToView(point);
}

vec3 Tool::getCameraXDirection()
{
    Camera * camera = Global::getInstance()->mainWindow()->viewport()->camera();

    return camera->getCameraXDirection();
}

vec3 Tool::getCameraPosition()
{
    Camera * camera = Global::getInstance()->mainWindow()->viewport()->camera();

    return camera->getPosition();
}

}
