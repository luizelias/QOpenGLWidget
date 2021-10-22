#ifndef CLONEBRUSH_H
#define CLONEBRUSH_H

#include "Brush.h"

namespace mush3d{

class CloneBrush : public Brush
{
public:
    CloneBrush();
    virtual ~CloneBrush();

    virtual void press  (const QPoint & mouse);
    virtual void drag   (const QPoint & mouse);
    virtual void release();

    virtual ToolType getType();
};

}

#endif // CLONEBRUSH_H
