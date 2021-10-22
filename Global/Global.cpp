#include "Global.h"
#include "Mesh.h"
#include "Viewport.h"

#include <maya/MQtUtil.h>


#include "CameraTool.h"
#include "MoveBrush.h"

namespace mush3d{

Global::Global()
{
    MGlobal::displayInfo("Global constructor");

    _mesh = new Mesh;

    _tools.append(new CameraTool);
    _tools.append(new MoveBrush);


    _tools[1]->setSelected(true);
}

Global::~Global()
{
    delete _mesh;

    int num_tools = _tools.size();
    for(int i=0; i<num_tools; i++)
    {
        delete _tools[i];
    }
    _tools.clear();

    MGlobal::displayInfo("Global destroctor");
}

Tool * Global::getSelectedTool()
{
    int num_tools = _tools.size();
    for(int i=0; i<num_tools; i++)
    {
        if(_tools[i]->isSelected())
        {
            return _tools[i];
        }
    }
    return nullptr;
}

Tool * Global::getTool(const ToolType &t)
{
    int num_tools = _tools.size();
    for(int i=0; i<num_tools; i++)
    {
        if(_tools[i]->getType() == t)
        {
            return _tools[i];
        }
    }
    return nullptr;
}

Global * Global::getInstance()
{
    static Global g;
    return & g;
}

mainWindowPtr Global::mainWindow()
{
    // initialize everything is window doesn't exist
    if(_mainWindow.isNull())
    {
        // create main window
        _mainWindow = new MainWindow(MQtUtil::mainWindow());
    }

    return _mainWindow;
}

void Global::cleanup()
{
    // kill main window
    if(!_mainWindow.isNull())
    {
        delete _mainWindow;
    }
}

// make the viewport context current
//void Global::makeCurrent()
//{
//    if(_mainWindow.isNull())
//        return;

//    _mainWindow->viewport()->makeCurrent();
//}

//void Global::doneCurrent()
//{
//    if(_mainWindow.isNull())
//        return;

//    _mainWindow->viewport()->doneCurrent();
//}

}

