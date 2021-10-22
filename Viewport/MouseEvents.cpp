#include "Viewport.h"
#include "Brush.h"
#include "BrushControls.h"

#include <maya/MGlobal.h>

namespace mush3d{

void Viewport::tabletEvent(QTabletEvent * event)
{
    Brush::pressure = event->pressure();
}

void Viewport::mousePressEvent(QMouseEvent * event)
{
    // mouse position
    Brush::shift = event->modifiers() & Qt::ShiftModifier;
    Brush::control = event->modifiers() & Qt::ControlModifier;
    bool alt = event->modifiers() & Qt::AltModifier;

    QPoint pos = event->pos();
    pos.setY(height() - pos.y());

    MGlobal::displayInfo(MString("pos 1") + pos.x() + ", " + pos.y());

    // save the press tool to restore at the end of event
    _pressTool = Global::getInstance()->getSelectedTool();

    // define the action
    _action = kNoAction;

    int button =(event->buttons() == Qt::LeftButton) ? 0 : (event->buttons() == Qt::MiddleButton) ? 1 : 2;
    Tool::button = button;

    if(alt)
    {
        _action = kCamera;
    }
    else if(button == 0)
    {
        _action= kSculpt;
    }
    else if(button == 1)
    {
        if(Brush::shift)
        {
            _action = kIntensity;
        }
        else
        {
            _action = kRadius;
        }
    }
    // do the action
    if(_action == kCamera)
    {
        Tool * tool = Global::getInstance()->getTool(ToolType::kCameraTool);
        tool->press(pos);
    }
    else if(_action == kIntensity)
    {
        _brushControls->setUiType(BrushControls::kIntensityUI);
        _brushControls->press(int(pos.x()),height() - int(pos.y()));
    }
    else if(_action == kRadius)
    {
        _brushControls->setUiType(BrushControls::kRadiusUI);
        _brushControls->press(int(pos.x()),height() - int(pos.y()));
    }
    else if(_action == kSculpt)
    {
        // switch tool if using hotkeys
        if(Brush::shift && (_pressTool->getType() != ToolType::kMaskingTool) && (_pressTool->getType() != ToolType::kWeightsTool))
        {
            //Global::getScene()->setSelectedTool(ToolType::kRelaxTool);
        }
        else if(Brush::control && (_pressTool->getType() == ToolType::kWrinkleTool))
        {
            //Global::getScene()->setSelectedTool(ToolType::kCloneTool);
        }

        Global::getInstance()->getSelectedTool()->press(pos);

        // update drawing
        update();
    }

    event->accept();
}

void Viewport::mouseMoveEvent(QMouseEvent *event)
{
    QPoint pos = event->pos();
    pos.setY(height() - pos.y());

    // do the action
    if(_action == kCamera)
    {
        Tool * tool = Global::getInstance()->getTool(ToolType::kCameraTool);
        tool->drag(pos);
        update();
    }
    else if(_action == kRadius)
    {
        _brushControls->drag(int(pos.x()), height() - int(pos.y()));
        _brushControls->repaint();
    }
    else if(_action == kIntensity)
    {
        _brushControls->drag(int(pos.x()), height() - int(pos.y()));
        _brushControls->repaint();
    }
    else if(_action == kSculpt)
    {
        Tool * tool = Global::getInstance()->getSelectedTool();
        tool->drag(pos);
        update();
    }
    else if(_action == kMouseMoving)
    {
        //        Tool::mousePosition = mouse->position();
        //        update();
    }

    event->accept();
}

void Viewport::mouseReleaseEvent(QMouseEvent *event)
{
    Tool * tool = Global::getInstance()->getSelectedTool();

    // do the action
    if(_action == kRadius || _action == kIntensity)
    {
        _brushControls->release();
    }
    else if(_action == kSculpt)
    {
        tool->release();
    }

    //Global::getScene()->setSelectedTool(_pressTool->getType());

    _action = kNoAction;

    // update drawing
    update();

    event->accept();
}

}
