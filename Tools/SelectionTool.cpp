#include "SelectionTool.h"
#include "GPU.h"
#include "ShaderManager.h"
#include "Mesh.h"
#include "SubdivisionLevel.h"

namespace mush3d{

SelectionTool::SelectionTool() : Tool()
{

}


SelectionTool::~SelectionTool()
{


}

ToolType SelectionTool::getType()
{
    return ToolType::kSelectionTool;
}

void SelectionTool::press(const QPoint & mouse)
{

}

void SelectionTool::drag(const QPoint & mouse)
{
    qDebug() << "dragging selection";
}

void SelectionTool::release()
{

}
}
