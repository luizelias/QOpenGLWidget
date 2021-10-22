#ifndef CREASEBRUSH_H
#define CREASEBRUSH_H

#include "Brush.h"

namespace mush3d{

class CreaseBrush : public Brush
{
public:
    CreaseBrush();
    virtual ~CreaseBrush();

    virtual void press  (const QPoint & mouse);
    virtual void drag   (const QPoint & mouse);
    virtual void release();

    virtual ToolType getType();

};

}

#endif // CREASEBRUSH_H
