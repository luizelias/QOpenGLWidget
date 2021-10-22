#ifndef LAYEREDITOR_H
#define LAYEREDITOR_H

#include <QWidget>
#include <QListWidget>

namespace mush3d{

class Layer;

class LayerEditor : public QWidget
{
    Q_OBJECT
public:
    LayerEditor(QWidget * parent = nullptr);

    void addLayer(Layer * layer);

    void setSelectedLayer(Layer * layer);

public slots:
    void currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

private:
    QListWidget * _layerList;

};

}

#endif // LAYEREDITOR_H
