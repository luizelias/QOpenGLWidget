#ifndef SELECTIONTOOL_H
#define SELECTIONTOOL_H

#include "Tool.h"

namespace mush3d{

class SelectionTool : public Tool
{
public:
    SelectionTool();
    virtual ~SelectionTool();

    virtual ToolType getType();

    virtual void press  (const QPoint & mouse);
    virtual void drag   (const QPoint & mouse);
    virtual void release();
};

}

#endif // SELECTIONTOOL_H
