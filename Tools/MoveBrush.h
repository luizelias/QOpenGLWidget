#ifndef MOVEBRUSH
#define MOVEBRUSH

#include "Brush.h"

namespace mush3d{

class MoveBrush : public Brush
{
public:
    MoveBrush();
    virtual ~MoveBrush();

    virtual void press  (const QPoint & mouse);
    virtual void drag   (const QPoint & mouse);
    virtual void release();

    virtual ToolType getType();

private:
    // for move by normal
    vec2 _pressPoint2D;
    vec2 _pressNormal2D;

};

}

#endif // MOVEBRUSH
