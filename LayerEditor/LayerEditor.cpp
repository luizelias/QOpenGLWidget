#include "LayerEditor.h"
#include "Layer.h"

#include <QVBoxLayout>

namespace mush3d{

LayerEditor::LayerEditor(QWidget * parent) : QWidget(parent)
{
    setMinimumHeight(600);

    QVBoxLayout * layout = new QVBoxLayout;
    setLayout(layout);


    _layerList = new QListWidget;

    layout->addWidget(_layerList);



    connect( _layerList, &QListWidget::currentItemChanged, this, &LayerEditor::currentItemChanged );
}

void LayerEditor::addLayer(Layer * layer)
{
    _layerList->addItem(layer);
    setSelectedLayer(layer);
}

void LayerEditor::setSelectedLayer(Layer * layer)
{
    for(int i = 0; i < _layerList->count(); ++i)
    {
        QListWidgetItem* item = _layerList->item(i);
        Layer * curr  = static_cast<Layer*>(item);

        if(curr == layer)
        {
            curr->setLayerSelected(true);
            curr->setSelected(true);
        }
        else
        {
            curr->setLayerSelected(false);
            curr->setSelected(false);
        }
    }
}

void LayerEditor::currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if(current)
    {
        Layer * curr  = static_cast<Layer*>(current);
        setSelectedLayer(curr);
    }
}

}
