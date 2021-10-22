#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <QOpenGLWidget>
#include <QMouseEvent>

#include "Camera.h"

namespace mush3d{

class MainWindow;
class Mesh;
class Tool;
class BrushControls;

class Viewport : public QOpenGLWidget
{
    Q_OBJECT
public:
    enum CursorType{kSculptCursor, kSelectCursor};
    enum Action{kNoAction, kRadius, kIntensity, kCamera, kSculpt, kSelect, kMouseMoving};

    explicit Viewport(MainWindow * mainWindow);
    ~Viewport();

    Camera * camera(){return &_camera;}

protected:
    void paintGL();
    void initializeGL();
    void resizeGL(int w, int h);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void tabletEvent(QTabletEvent *event);
    void setViewportCursor(CursorType t);

    void keyPressEvent(QKeyEvent *event);

private:
    MainWindow * _mainWindow;
    BrushControls *  _brushControls;

    Tool * _pressTool;
    Camera _camera;
    Action _action;
};
}

#endif // VIEWPORT_H
