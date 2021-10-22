#include "Types.h"
#include "Utils.h"

namespace mush3d{

Plane::Plane(const vec3 & center, const vec3 & normal): _center(center), _normal(normal)
{}

void Plane::setCenter(const vec3 & point){_center=point;}
void Plane::setNormal(const vec3 & norm){_normal=norm;}
vec3 Plane::getNormal(){return _normal;}

vec3 Plane::intersect(const Ray & ray)
{
    float D = -(_normal.x()*_center.x() + _normal.y()*_center.y() + _normal.z()*_center.z());
    float denominator = _normal.x()*ray.direction().x() + _normal.y()*ray.direction().y() + _normal.z()*ray.direction().z();
    float t = -(D + _normal.x()*ray.getNear().x() + _normal.y()*ray.getNear().y() + _normal.z()*ray.getNear().z()) / denominator;
    vec3 scaledLineDirection = vec3(ray.direction().x()*t,ray.direction().y()*t,ray.direction().z()*t);
    return (ray.getNear() + scaledLineDirection);
}

BrushData::BrushData() : pressHit(false), hitTriangle(-1), enoughtDistance(false), numStrokes(0), count(0), numVerticesInSphere(0),
    closestVertex(Utils::INVALID_ID)
{

}

// PARTICLE //
Particle::Particle() : vertex_id(0), locked(false), weight(0.01f), inverseWeight(1.0f / weight)
{
    for(int i=0; i<24; i++)
    {
        springs[i] = Utils::INVALID_ID;
    }
}

void Particle::addSpring(uint idx, uint type)
{
    for(int i=0; i<24; i++)
    {
        if(springs[i] == Utils::INVALID_ID)
        {
            springs[i] = idx;
            springType[i] = type;
            return;
        }
    }
}

int Particle::getNumSprings()
{
    for(int i=0; i<24; i++)
    {
        if(springs[i] == Utils::INVALID_ID)
        {
            return i;
        }
    }
    return 24;
}

uint Particle::getSpring(int idx)
{
    return springs[idx];
}

}
