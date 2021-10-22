#include "DeformerButton.h"
#include "DeformerManager.h"

#include <QMenu>

namespace mush3d{

DeformerButton::DeformerButton(QWidget *parent) : QPushButton("<<", parent)
{

}

void DeformerButton::mousePressEvent(QMouseEvent * event)
{
    QPoint pos = this->mapToGlobal(event->pos());

    QMenu * menu = new QMenu();

    QAction * createNew = new QAction("Create New", this);

    menu->addAction(createNew);

    QAction* item = menu->exec(pos);

    if(item == createNew)
    {
        DeformerManager::getInstance()->createNewDeformer();
    }

    event->accept();
}

}
