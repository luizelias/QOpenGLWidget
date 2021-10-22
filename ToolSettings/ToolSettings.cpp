#include "ToolSettings.h"
#include "MainWindow.h"
#include "DeformerButton.h"
#include "LayerEditor.h"
#include "DeformerManager.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>

namespace mush3d{

ToolSettings::ToolSettings(MainWindow * w) : QWidget(w), _mainWindow(w)
{
    // layout
    setFixedWidth(200);

    createWidgets();
    createLayouts();
    createConnections();
}

void ToolSettings::createWidgets()
{
    _deformerLabel = new QLabel("Deformer: ", this);

    _deformerLineEdit = new QLineEdit(this);
    _deformerLineEdit->setMaximumWidth(200);
    _deformerLineEdit->setReadOnly(true);

    _deformerBnt = new DeformerButton(this);
    _deformerBnt->setMaximumWidth(25);

    _addLayerBnt = new QPushButton("+");

    _layerEditor = new LayerEditor(this);

}

void ToolSettings::createLayouts()
{
    // MAIN
    QVBoxLayout * mainLayout = new QVBoxLayout();
    this->setLayout(mainLayout);

    mainLayout->setAlignment(Qt::AlignTop);
    mainLayout->setContentsMargins(0,0,0,0);

    // DEFORMER
    QHBoxLayout * deformerLayout = new QHBoxLayout;

    // ATTACH WIDGETS TO LAYOUTS
    deformerLayout->addWidget(_deformerLabel);
    deformerLayout->addWidget(_deformerLineEdit);
    deformerLayout->addWidget(_deformerBnt);
    mainLayout->addLayout(deformerLayout);

    mainLayout->addWidget(_addLayerBnt);

    mainLayout->addWidget(_layerEditor);

}

void ToolSettings::createConnections()
{
    connect(_addLayerBnt, &QPushButton::clicked, [=]() {

        DeformerManager::getInstance()->createLayer();

    });

}

}
