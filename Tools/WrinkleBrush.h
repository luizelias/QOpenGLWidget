#ifndef WRINKLEBRUSH_H
#define WRINKLEBRUSH_H

#include "Brush.h"

namespace mush3d{

class WrinkleBrush : public Brush
{
public:
    WrinkleBrush();
    virtual ~WrinkleBrush();

    virtual void press  (const QPoint & mouse);
    virtual void drag   (const QPoint & mouse);
    virtual void release();

    virtual ToolType getType();
};

}

#endif // WRINKLEBRUSH_H
