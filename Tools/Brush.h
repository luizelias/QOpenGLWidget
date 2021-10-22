#ifndef BRUSH_H
#define BRUSH_H

#include "Tool.h"
#include "GLBuffer.h"

#include <QtGui/QOpenGLTexture>

namespace mush3d{

class SculptLayer;
class SculptSample;
class Mesh;
class SubdivisionLevel;

class Brush : public Tool
{
public:
    static float pressure;
    static float intensity;
    static bool shift;
    static bool control;
    static int radius2d;
    static float radius3d;
    static mat4 matrix;
    static QOpenGLTexture * squareAlpha;
    static QOpenGLTexture * dotAlpha;
    static vec3 moveDelta;
    static vec3 hitPoint;
    static bool topological;

    static int wrinkleIterations;
    static float wrinkleStructuralForce;
    static float wrinkleFlexionForce;
    static float wrinkleShearForce;
    static float wrinkleDeltaTime;

    static int cloneIterations;
    static int smoothIterations;
    static int cloneSmoothBaseIterations;


    static int max2dRadius;

public:
    Brush();
    virtual ~Brush();

    virtual void press  (const QPoint & mouse);
    virtual void drag   (const QPoint & mouse);
    virtual void release();

protected:
    virtual bool canSculpt();

    QPoint _mousePress;
    QPoint _mouseDrag;

    Mesh *  _mesh;
    SubdivisionLevel * _level;

    Plane _plane;

    vec3 _hitPoint;
    vec3 _hitNormal;

    QVector<QPoint> _pixels; // pixels to sculpt

    vec4Array _pressPoints;
};

}

#endif // BRUSH_H
