#ifndef CAMERATOOL_H
#define CAMERATOOL_H

#include "Tool.h"

namespace mush3d {

class Camera;

class CameraTool : public Tool
{
public:
    static float zoomSpeed;

    CameraTool();
    virtual ~CameraTool();

    virtual ToolType getType();

    virtual void press  (const QPoint & mouse);
    virtual void drag   (const QPoint & mouse);
    virtual void release();

private:
    void drag2d(const QPoint & mouse, Camera * camera);


    private:
        float definePanSize();

private:
    QPoint pressPosition;
    QPoint lastPosition;

    float press_width;
    float press_height;

};

}

#endif // CAMERATOOL_H
