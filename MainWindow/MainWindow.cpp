#include "MainWindow.h"
#include "Viewport.h"
#include "ToolSettings.h"

#include <QVBoxLayout>
#include <QCoreApplication>

#include <maya/MGlobal.h>

namespace mush3d{

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    // layout the elements
    QWidget * CentralWidget = new QWidget();
    setCentralWidget(CentralWidget);

    QHBoxLayout * layout = new QHBoxLayout;
    CentralWidget->setLayout(layout);

    // tools settings
    _toolSettings = new ToolSettings(this);
    layout->addWidget(_toolSettings);

    // viewport
    _viewport = new Viewport(this);
    layout->addWidget(_viewport);

    // set focus
    setFocusPolicy(Qt::StrongFocus);

    MGlobal::displayInfo("MainWIndow constructor");
}

MainWindow::~MainWindow()
{
    MGlobal::displayInfo("MainWIndow destructor");
}

}

