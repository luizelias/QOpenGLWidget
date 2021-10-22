#ifndef CAMERA_H
#define CAMERA_H

#include <QString>

#include "Types.h"
#include "Aabb.h"

namespace mush3d{

class Camera
{
public:
    Camera();
    virtual ~Camera();

    void setWidth(int w);
    void setHeight(int h);

    int getWidth();
    int getHeight();

    mat4 getProjection();
    vec3 getNearPoint (float x, float y);
    vec3 getFarPoint  (float x, float y);
    Ray  getRay       (float x, float y);

    void frameAabb(const Aabb & aabb);
    vec3 getCameraXDirection();
    vec3 viewDirection();
    vec2 worldToView(const vec3 & point);
    float viewToWorld(int size, const vec3 referencePoint);

    void setPointOfInterest(const vec3 &pos);

    vec3 getPosition();

    mat4 getModelView();
    void setModelView(const mat4 m){_modelView=m;}

public:
    vec3       _position;
    vec3       _rotation;
    vec3       _pointOfInterest;

    bool       _isLocked;

    int _width;
    int _height;

    float _focalLength = 35.0;

    mat4 _modelView;
};

}

#endif // CAMERA_H
