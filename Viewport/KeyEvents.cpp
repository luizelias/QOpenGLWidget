#include "Viewport.h"
#include "Mesh.h"
#include "Camera.h"
#include "Mesh.h"

#include <maya/MGlobal.h>

namespace mush3d{

void Viewport::keyPressEvent(QKeyEvent * event)
{

    if(event->key() == Qt::Key_F)
    {
        _camera.frameAabb(Global::getInstance()->mesh()->getAabb());

        update();

        event->accept();
    }
}




}
