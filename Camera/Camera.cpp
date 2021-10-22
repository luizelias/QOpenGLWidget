#include "Camera.h"
#include "Global.h"
#include "CameraTool.h"

namespace mush3d{

Camera::Camera() : _pointOfInterest(0,0,0), _width(1), _height(1)
{

}

Camera::~Camera()
{

}

void Camera::setWidth(int w)
{
    _width = w;
}

void Camera::setHeight(int h)
{
    _height = h;
}

int Camera::getWidth()
{
    return _width;
}

int Camera::getHeight()
{
    return _height;
}

mat4 Camera::getProjection()
{
    mat4 proj;

    if(_height < 1) return proj;

    float aspect = (float)_width / _height;
    proj.perspective(_focalLength, aspect, 0.001f, 1000.0f);

    return proj;
}

vec3 Camera::getNearPoint(float x, float y)
{
    float width = float(_width);
    float height = float(_height);
    return vec3(x,y,0.0f).unproject(_modelView, getProjection(), QRect(0, 0, width, height));
}

vec3 Camera::getFarPoint(float x, float y)
{
    float width = float(_width);
    float height = float(_height);
    return vec3(x,y,1.0f).unproject(_modelView, getProjection(), QRect(0, 0, width, height));
}

Ray Camera::getRay(float x, float y)
{
    vec3 nearP = getNearPoint(x,y);
    vec3 farP = getFarPoint(x,y);
    return Ray(nearP, farP);
}

void Camera::frameAabb(const Aabb &aabb)
{
    vec3 aabbPos = aabb.getCenter();

    float size = (aabb._max-aabb._min).length();

    _pointOfInterest = aabbPos;
    _position = -aabbPos;
    _position += vec3(0.0f,0.0f,-1.0f)*size;

    // rebuild model matrix
    _modelView = mat4();
    _modelView.translate(_position);
    _modelView.translate(_pointOfInterest);
    _modelView.rotate(_rotation.x(), QVector3D(1.0f, 0.0f, 0.0f));
    _modelView.rotate(_rotation.y(), QVector3D(0.0f, 1.0f, 0.0f));
    _modelView.translate(-_pointOfInterest);

    // fix camera zoom speed
    CameraTool::zoomSpeed = size*0.001f;
}

vec3 Camera::getCameraXDirection()
{
    mat4 transform = _modelView.inverted();
    return transform.column(0).toVector3D().normalized();
}

vec3 Camera::viewDirection()
{
    mat4 transform = _modelView.inverted();
    return transform.column(2).toVector3D().normalized();
}

vec2 Camera::worldToView(const vec3 &point)
{
    float width = float(_width);
    float height = float(_height);

    mat4 proj = getProjection();
    mat4 MVP = proj * _modelView;

    vec4 P = MVP * vec4(point.x(),point.y(), point.z(), 1.0f);

    vec2 point2d = vec2(P.x()/P.w(), P.y()/P.w());

    point2d.setX( (point2d.x()/2.0f + 0.5f) * width);
    point2d.setY( (point2d.y()/2.0f + 0.5f) * height);

    return point2d;
}

float Camera::viewToWorld(int size, const vec3 referencePoint)
{
    float width = float(_width);
    float height = float(_height);

    int w = width/2;
    int h =  height/2;

    vec3 nearP = getNearPoint(w,h);
    vec3 farP = getFarPoint(w,h);

    Plane plane(referencePoint, (nearP-farP).normalized());

    Ray ray1(nearP, farP);

    vec3 p1 = plane.intersect(ray1);

    nearP = getNearPoint(w + size,h);
    farP = getFarPoint(w + size,h);

    Ray ray2(nearP, farP);

    vec3 p2 = plane.intersect(ray2);

    return p1.distanceToPoint(p2);
}

void Camera::setPointOfInterest(const vec3 & pos)
{
    // resulting camera matrix rotating around old pivot
    mat4 oldModel = mat4();
    oldModel.translate(_position);
    oldModel.translate(_pointOfInterest);
    oldModel.rotate(_rotation.x(), QVector3D(1.0f, 0.0f, 0.0f));
    oldModel.rotate(_rotation.y(), QVector3D(0.0f, 1.0f, 0.0f));
    oldModel.translate(-_pointOfInterest);

    // resulting camera matrix rotating around new pivot
    _pointOfInterest = pos;

    mat4 newModel = mat4();
    newModel.translate(_position);
    newModel.translate(_pointOfInterest);
    newModel.rotate(_rotation.x(), QVector3D(1.0f, 0.0f, 0.0f));
    newModel.rotate(_rotation.y(), QVector3D(0.0f, 1.0f, 0.0f));
    newModel.translate(-_pointOfInterest);

    // update camera position
    mat4 diference = oldModel * newModel.inverted();
    vec4 newPos = vec4(_position.x(), _position.y(), _position.z(), 1.0f);
    newPos = diference * newPos;

    _position = vec3(newPos.x(), newPos.y(), newPos.z());
}

vec3 Camera::getPosition()
{
    mat4 transform = _modelView.inverted();
    return (transform * vec4(0,0,0,1)).toVector3D();
}

mat4 Camera::getModelView()
{
    return _modelView;
}

}
