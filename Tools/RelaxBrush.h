#ifndef RELAXBRUSH_H
#define RELAXBRUSH_H


#include "Brush.h"

namespace mush3d{

class RelaxBrush : public Brush
{
public:
    RelaxBrush();
    virtual ~RelaxBrush();

    virtual void press  (const QPoint & mouse);
    virtual void drag   (const QPoint & mouse);
    virtual void release();

    virtual ToolType getType();

};

}

#endif // RELAXBRUSH_H
