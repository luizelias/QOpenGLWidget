#ifndef CLAYBRUSH_H
#define CLAYBRUSH_H


#include "Brush.h"


namespace mush3d{

class ClayBrush : public Brush
{
public:
    ClayBrush();
    virtual ~ClayBrush();

    virtual void press  (const QPoint & mouse);
    virtual void drag   (const QPoint & mouse);
    virtual void release();

    virtual ToolType getType();
};

}

#endif // CLAYBRUSH_H
