#include "Aabb.h"
#include "Global.h"

Aabb::Aabb() : _initialized(false)
{}

Aabb::~Aabb()
{}

vec3 Aabb::getCenter() const { return (_max+_min)/2.0f; }


void Aabb::expand(const vec4 & point)
{
    expand(point.toVector3D());
}

void Aabb::expand(const vec3 &point)
{
    if(_initialized == false)
    {
        _min = point;
        _max = point;
        _initialized = true;
        return;
    }

    if (point.x()>_max.x())
        _max.setX(point.x());
    if (point.x()<_min.x())
        _min.setX(point.x());
    if (point.y()>_max.y())
        _max.setY(point.y());
    if (point.y()<_min.y())
        _min.setY(point.y());
    if (point.z()>_max.z())
        _max.setZ(point.z());
    if (point.z()<_min.z())
        _min.setZ(point.z());
}


void Aabb::expand(const Aabb &aabb)
{
    if (aabb._max.x()>_max.x())
        _max.setX(aabb._max.x());
    if (aabb._min.x()<_min.x())
        _min.setX(aabb._min.x());
    if (aabb._max.y()>_max.y())
        _max.setY(aabb._max.y());
    if (aabb._min.y()<_min.y())
        _min.setY(aabb._min.y());
    if (aabb._max.z()>_max.z())
        _max.setZ(aabb._max.z());
    if (aabb._min.z()<_min.z())
        _min.setZ(aabb._min.z());
}

void Aabb::setScale(float s)
{
    vec3 newMax = _min + (_max-_min) * s;
    vec3 newMin = _max + (_min-_max) * s;

    _max = newMax;
    _min = newMin;
}

float Aabb::getWidth()
{
    return _max.x() - _min.x();
}

float Aabb::getHeight()
{
    return _max.y() - _min.y();
}

float Aabb::getDepth()
{
    return _max.z() - _min.z();
}

vec3 Aabb::getMinPoint()
{
    return _min;
}

vec3 Aabb::getMaxPoint()
{
    return _max;
}

bool Aabb::isFlatt()
{
    if(getWidth() < 0.0001f) return true;
    if(getHeight() < 0.0001f) return true;
    if(getDepth() < 0.0001f) return true;
    return false;
}

void Aabb::draw(const mat4 &mvp)
{
//    Global::drawManager()->drawLine(vec3(_min.x(), _min.y(), _min.z()),vec3(_max.x(), _min.y(), _min.z()),vec3(1,0,1),1.0f, 1, mvp);
//    Global::drawManager()->drawLine(vec3(_max.x(), _min.y(), _min.z()),vec3(_max.x(), _max.y(), _min.z()),vec3(1,0,1),1.0f, 1, mvp);
//    Global::drawManager()->drawLine(vec3(_max.x(), _max.y(), _min.z()),vec3(_min.x(), _max.y(), _min.z()),vec3(1,0,1),1.0f, 1, mvp);
//    Global::drawManager()->drawLine(vec3(_min.x(), _max.y(), _min.z()),vec3(_min.x(), _min.y(), _min.z()),vec3(1,0,1),1.0f, 1, mvp);
//    Global::drawManager()->drawLine(vec3(_min.x(), _min.y(), _min.z()),vec3(_min.x(), _min.y(), _max.z()),vec3(1,0,1),1.0f, 1, mvp);
//    Global::drawManager()->drawLine(vec3(_min.x(), _min.y(), _max.z()),vec3(_max.x(), _min.y(), _max.z()),vec3(1,0,1),1.0f, 1, mvp);
//    Global::drawManager()->drawLine(vec3(_max.x(), _min.y(), _max.z()),vec3(_max.x(), _min.y(), _min.z()),vec3(1,0,1),1.0f, 1, mvp);
//    Global::drawManager()->drawLine(vec3(_max.x(), _min.y(), _max.z()),vec3(_max.x(), _max.y(), _max.z()),vec3(1,0,1),1.0f, 1, mvp);
//    Global::drawManager()->drawLine(vec3(_max.x(), _max.y(), _max.z()),vec3(_max.x(), _max.y(), _min.z()),vec3(1,0,1),1.0f, 1, mvp);
//    Global::drawManager()->drawLine(vec3(_max.x(), _max.y(), _max.z()),vec3(_min.x(), _max.y(), _max.z()),vec3(1,0,1),1.0f, 1, mvp);
//    Global::drawManager()->drawLine(vec3(_min.x(), _max.y(), _max.z()),vec3(_min.x(), _max.y(), _min.z()),vec3(1,0,1),1.0f, 1, mvp);
//    Global::drawManager()->drawLine(vec3(_min.x(), _max.y(), _max.z()),vec3(_min.x(), _min.y(), _max.z()),vec3(1,0,1),1.0f, 1, mvp);
}
