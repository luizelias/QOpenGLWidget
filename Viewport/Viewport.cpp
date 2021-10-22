#include "Viewport.h"
#include "MainWindow.h"
#include "Global.h"
#include "GLFunctions.h"
#include "DrawManager.h"
#include "Mesh.h"
#include "ShaderManager.h"
#include "GLBuffer.h"
#include "BrushControls.h"

#include <QVBoxLayout>

namespace mush3d
{

Viewport::Viewport(MainWindow * mainWindow) :
    QOpenGLWidget(mainWindow), _mainWindow(mainWindow)
{
    QVBoxLayout * layout = new QVBoxLayout;
    setLayout(layout);

    setViewportCursor(kSculptCursor);
    layout->setContentsMargins(0, 0, 0, 0);

    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);

    setMinimumWidth(640);
    setMinimumHeight(480);

    // add brush control widget
    _brushControls = new BrushControls(this);
    layout->addWidget(_brushControls);

    setSizePolicy(QSizePolicy::Expanding , QSizePolicy::Expanding );

    //    QCoreApplication::setAttribute(Qt::AA_CompressHighFrequencyEvents);
    //    QCoreApplication::setAttribute(Qt::AA_CompressTabletEvents);
    //    QCoreApplication::setAttribute(Qt::AA_SynthesizeMouseForUnhandledTabletEvents, false);

    //    setAttribute(Qt::WA_TabletTracking);

    // make it accepts drops from outliner nodes for cameras
    //    setAcceptDrops(true);
}

Viewport::~Viewport()
{
    MGlobal::displayInfo("Viewport destructor");

}

void Viewport::initializeGL()
{
    // initialize shaders
    ShaderManager::getInstance()->initialize();

    auto gl = GLFunctions::getInstance();

    gl->initialize();

    gl->glEnable(GL_DEPTH_TEST);
    gl->glEnable(GL_CULL_FACE);
    gl->glCullFace(GL_BACK);

    //    // init brush alphas
    //    Brush::squareAlpha = new QOpenGLTexture(QOpenGLTexture::Target2D);
    //    QImage squareImg(":/alphas/Resources/alphas/squareAlpha.png");
    //    Brush::squareAlpha->setData(squareImg.mirrored(), QOpenGLTexture::DontGenerateMipMaps);

    //    Brush::dotAlpha = new QOpenGLTexture(QOpenGLTexture::Target2D);
    //    QImage dotImg(":/alphas/Resources/alphas/dotAlpha.jpg");
    //    Brush::dotAlpha->setData(dotImg.mirrored(), QOpenGLTexture::DontGenerateMipMaps);

    //    _isInitialized = true;

//    vec4Array data;
//    data.append(vec4(12,12,12,12));

//    GLBuffer buffer;
//    buffer.setData(data, 0);

//    vec4Array res;
//    buffer.getData(res, 0);

//    if(res.length() == 1)
//    {
//        MGlobal::displayInfo(MString("res: ") + res[0].x());
//    }

    gl->check();


}

void Viewport::paintGL()
{
    auto gl = GLFunctions::getInstance();

    gl->glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mat4 modelView = _camera.getModelView();
    mat4 projection = _camera.getProjection();
    mat3 normal = modelView.normalMatrix();

    Mesh * mesh  = Global::getInstance()->mesh();

    if(mesh->getNumVertices() > 0)
    {
        DrawManager::getInstance()->drawMesh(mesh, modelView, normal, projection*modelView);
    }

    gl->check();
}

void Viewport::resizeGL(int w, int h)
{
    auto gl = GLFunctions::getInstance();

    gl->glViewport(0, 0, w, h);

    if(w < 1) w = 1;
    if(h < 1) h = 1;

    _camera.setWidth(w);
    _camera.setHeight(h);
}

void Viewport::setViewportCursor(CursorType t)
{
    if (t==kSculptCursor)
    {
        static const char *const cursor_xpm[] = {
            "6 6 3 1",
            " 	c None",
            ".	c #000000",
            "+	c #FF0000",
            "......",
            ".++++.",
            ".+  +.",
            ".+  +.",
            ".++++.",
            "......"};

        QCursor myCursor = QCursor(QPixmap(cursor_xpm));
        setCursor(myCursor);
    }
    else
    {
        setCursor(Qt::ArrowCursor);
    }
}



}
