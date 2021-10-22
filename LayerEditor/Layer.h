#ifndef LAYER_H
#define LAYER_H

#include <QListWidgetItem>
#include <maya/MObject.h>
#include <maya/MFnIntArrayData.h>
#include <maya/MFnPointArrayData.h>

namespace mush3d{

class Layer : public QListWidgetItem
{
public:
    Layer(int logicalIndex, MObject deformer);

    int logicalIndex(){return _logicalIndex;}

    void setLayerSelected(bool s);

private:
    MObject _deformer;
    int _logicalIndex;
};

}

#endif // LAYER_H
