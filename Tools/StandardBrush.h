#ifndef STANDARDBRUSH_H
#define STANDARDBRUSH_H

#include "Brush.h"

namespace mush3d{

class StandardBrush : public Brush
{
public:
    StandardBrush();
    virtual ~StandardBrush();

    virtual void press  (const QPoint & mouse);
    virtual void drag   (const QPoint & mouse);
    virtual void release();

    virtual ToolType getType();

};

}

#endif // STANDARDBRUSH_H
