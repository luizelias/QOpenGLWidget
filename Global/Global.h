#ifndef GLOBAL_H
#define GLOBAL_H

#include "MainWindow.h"
#include "Types.h"

#include <QtCore/QPointer>

namespace mush3d{

typedef QPointer<MainWindow> mainWindowPtr;


class Tool;
class Mesh;

class Global
{
private:
    Global();

public:
    ~Global();

    // remove copy and assign operator
    Global(const Global&) = delete;
    void operator=(Global const&) = delete;

    static Global * getInstance();

    mainWindowPtr mainWindow();

    void cleanup();

    // set
    void setWireframe(bool b){_wireframeEnable = b;}
    void setWireframeIntensity(float i){_wireframeIntensity = i;}
    void setHardNormalIntensity(float i){_hardNormalIntensity = i;}

    // get
    bool wireframeEnbaled(){return _wireframeEnable;}
    double wireframeIntensity(){return _wireframeIntensity;}
    double hardNormalIntensity(){return _hardNormalIntensity;}

    Tool * getTool(const ToolType & t);
    Tool * getSelectedTool();

    Mesh * mesh(){return _mesh;}

    void setMorphPoints(const vec4Array & points){_morphPoints = points;};
    vec4Array & getMorphPoints(){return _morphPoints;}

private:
    mainWindowPtr _mainWindow;

    bool _wireframeEnable = false;
    float _wireframeIntensity = 0.0f;
    float _hardNormalIntensity = 0.0f;

    QVector<Tool*> _tools;

    Mesh * _mesh;

    vec4Array _morphPoints;

};

}

#endif // GLOBAL_H
