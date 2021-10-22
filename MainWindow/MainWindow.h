#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace mush3d{

class Viewport;
class ToolSettings;


class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow();


    Viewport * viewport(){return _viewport;}
    ToolSettings * toolSettings(){return _toolSettings;}

signals:



private:
    Viewport * _viewport;
    ToolSettings * _toolSettings;


};

}

#endif // MAINWINDOW_H
