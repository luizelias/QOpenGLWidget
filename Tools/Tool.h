#ifndef TOOL_H
#define TOOL_H

#include "Types.h"

namespace mush3d{

class Camera;

class Tool
{
public:
    static int button;

public:
    Tool();
    virtual ~Tool();

    virtual void press  (const QPoint & mouse)=0;
    virtual void drag   (const QPoint & mouse)=0;
    virtual void release()=0;

    virtual void draw(Camera * camera){};

    virtual ToolType getType()=0;

    void setSelected(bool s);
    bool isSelected();

    // helper functions that uses the selected viewport and the selected camera
    static Ray   getRay(const QPoint & point);
    static void  setPointOfInterest(const vec3 & point);
    static float viewToWorld(int size, const vec3 referencePoint);
    static vec3  nearPoint(const QPoint & point);
    static vec3  farPoint(const QPoint & point);
    static vec2  worldToView(const vec3 & point);
    static vec3  getCameraXDirection();
    static vec3  getCameraPosition();

protected:
    bool _isSelected;

};

}

#endif // TOOL_H
