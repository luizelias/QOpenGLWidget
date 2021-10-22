#ifndef INFLATEBRUSH_H
#define INFLATEBRUSH_H

#include "Brush.h"

namespace mush3d{

class InflateBrush : public Brush
{
public:
    InflateBrush();
    virtual ~InflateBrush();

    virtual void press  (const QPoint & mouse);
    virtual void drag   (const QPoint & mouse);
    virtual void release();

    virtual ToolType getType();

};

}

#endif // INFLATEBRUSH_H
