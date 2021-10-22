#ifndef ERASEBRUSH_H
#define ERASEBRUSH_H

#include "Brush.h"

namespace mush3d{

class EraseBrush : public Brush
{
public:
    EraseBrush();
    virtual ~EraseBrush();

    virtual void press  (const QPoint & mouse);
    virtual void drag   (const QPoint & mouse);
    virtual void release();

    virtual ToolType getType();

};

}

#endif // ERASEBRUSH_H
