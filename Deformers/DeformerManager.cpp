#include "DeformerManager.h"
#include "SculptDeformer.h"
#include "Global.h"
#include "ToolSettings.h"
#include "LayerEditor.h"
#include "Layer.h"

#include <maya/MGlobal.h>

namespace mush3d{

DeformerManager::DeformerManager()
{

}

DeformerManager * DeformerManager::getInstance()
{
    static DeformerManager d;
    return & d;
}

MStatus DeformerManager::createNewDeformer()
{
    MStatus status;

    // create a new deformer
    MString result;
    status = MGlobal::executeCommand("mush3dSculptCmd", result);

    if(!status)
    {
        MGlobal::displayError("Error creating deformer 1");
        _deformer = MObject::kNullObj;
        return status;
    }

    MSelectionList selList;
    status = selList.add(result);

    if(!status || (selList.length() != 1))
    {
        MGlobal::displayError("Error creating deformer 2");
        _deformer = MObject::kNullObj;
        return status;
    }

    status = selList.getDependNode(0, _deformer);

    if(!status)
    {
        MGlobal::displayError("Error creating deformer 3");
        _deformer = MObject::kNullObj;
        return status;
    }

    MFnDependencyNode fnNode(_deformer);

    if (fnNode.typeId() != SculptDeformer::id)
    {
        MGlobal::displayError("Error creating deformer");
        _deformer = MObject::kNullObj;
        return status;
    }

    MGlobal::displayInfo(MString("new deformer created: ") + fnNode.name());

    return status;
}

void DeformerManager::createLayer()
{
    MObjectHandle handle(_deformer);

    if(handle.isValid() == false)
    {
        MGlobal::displayError("deformer has been deleted");
        return;
    }

    MFnDependencyNode fnNode(_deformer);
    MString cmd("mush3dCreateLayerCmd ");
    cmd += "-d ";
    cmd += fnNode.name();
    cmd += ";";

    MStatus status;
    int logicalIndex = -1;
    status = MGlobal::executeCommand(cmd, logicalIndex);

    if(!status || (logicalIndex < 0))
    {
        MGlobal::displayError("can't create layer");
    }

    // create the layer
    Layer * layer = new Layer(logicalIndex, _deformer);

    Global::getInstance()->mainWindow()->toolSettings()->layerEditor()->addLayer(layer);
}



}
