#ifndef CLOTHBRUSH_H
#define CLOTHBRUSH_H

#include "Brush.h"

namespace mush3d{

class ClothBrush : public Brush
{
public:
    ClothBrush();
    virtual ~ClothBrush();

    virtual void press  (const QPoint & mouse);
    virtual void drag   (const QPoint & mouse);
    virtual void release();

    virtual ToolType getType();
};

}

#endif // CLOTHBRUSH_H
