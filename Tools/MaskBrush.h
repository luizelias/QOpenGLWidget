#ifndef MASKBRUSH_H
#define MASKBRUSH_H

#include "Brush.h"

namespace mush3d{

class MaskBrush : public Brush
{
public:
    MaskBrush();
    virtual ~MaskBrush();

    virtual void press  (const QPoint & mouse);
    virtual void drag   (const QPoint & mouse);
    virtual void release();

    virtual ToolType getType();
};

}

#endif // MASKBRUSH_H
