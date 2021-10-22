#include "CameraTool.h"
#include "Global.h"
#include "MainWindow.h"
#include "Viewport.h"

#include <maya/MGlobal.h>

namespace mush3d {

float CameraTool::zoomSpeed = 0.02f;

CameraTool::CameraTool() : Tool()
{

}


CameraTool::~CameraTool()
{

}

ToolType CameraTool::getType()
{
    return ToolType::kCameraTool;
}

void CameraTool::press(const QPoint & mouse)
{
    lastPosition = mouse;
    pressPosition = mouse;
}

void CameraTool::drag(const QPoint & mouse)
{
    Camera * camera = Global::getInstance()->mainWindow()->viewport()->camera();

    float diffx= mouse.x() - lastPosition.x();
    float diffy= mouse.y() - lastPosition.y();

    lastPosition = mouse;

    vec3 camera_pos = camera->_position;
    vec3 camera_rot = camera->_rotation;

    if(button == 0)
    {
        camera_rot.setX(camera_rot.x() - (( 0.5f * diffy) ));
        camera_rot.setY(camera_rot.y() + (( 0.5f * diffx) ));
    }
    else if(button == 1)
    {
        float pixel3dSize = definePanSize();

        camera_pos.setX(camera_pos.x() + (float)diffx*pixel3dSize);
        camera_pos.setY(camera_pos.y() + (float)diffy*pixel3dSize);
    }
    else if(button == 2)
    {
        if(abs(diffx) > abs(diffy))
        {
            camera_pos.setZ(camera_pos.z() + zoomSpeed * diffx);
        }
        else
        {
            camera_pos.setZ(camera_pos.z() - zoomSpeed * diffy);
        }
    }

    // create model view
    vec3 pointOfInterest = camera->_pointOfInterest;

    mat4 modelView = mat4();
    modelView.translate(camera_pos);
    modelView.translate(pointOfInterest);
    modelView.rotate(camera_rot.x(), QVector3D(1.0f, 0.0f, 0.0f));
    modelView.rotate(camera_rot.y(), QVector3D(0.0f, 1.0f, 0.0f));
    modelView.translate(-pointOfInterest);

    camera->_position = camera_pos;
    camera->_rotation = camera_rot;
    camera->setModelView(modelView);
}

void CameraTool::release()
{

}

float CameraTool::definePanSize()
{
    Viewport * viewport = Global::getInstance()->mainWindow()->viewport();
    Camera * camera = viewport->camera();

    vec3 pointOfInterest = camera->_pointOfInterest;

    // get the camera x directnion
    vec3 cameraX = camera->getCameraXDirection();

    vec2 p0 = camera->worldToView(pointOfInterest);
    vec2 p1 = camera->worldToView(pointOfInterest + cameraX);

    float dist = p0.distanceToPoint(p1);

    if(dist > 0.0f)
    {
        return 1.0f / dist;
    }

    return 1.0f;
}

}
