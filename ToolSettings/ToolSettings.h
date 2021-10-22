#ifndef TOOLSETTINGS_H
#define TOOLSETTINGS_H

#include <QWidget>
#include <QPushButton>

class QLabel;
class QLineEdit;

namespace mush3d{


class MainWindow;
class DeformerButton;
class LayerEditor;

class ToolSettings : public QWidget
{
    Q_OBJECT

public:
    explicit ToolSettings(MainWindow * w);

    LayerEditor * layerEditor(){return _layerEditor;}

private:
    void createWidgets();
    void createLayouts();
    void createConnections();

private:
    MainWindow * _mainWindow;
    QLabel * _deformerLabel;
    QLineEdit * _deformerLineEdit;
    DeformerButton * _deformerBnt;
    QPushButton * _addLayerBnt;
    LayerEditor * _layerEditor;

};

}


#endif // TOOLSETTINGS_H
