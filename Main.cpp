#include <maya/MFnPlugin.h>
#include <maya/MGlobal.h>
#include "CreateMainWindow.h"
#include "CreateSculptDeformer.h"
#include "CreateLayer.h"
#include "Global.h"

#include "SculptDeformer.h"

MString SculptDeformerName = "mush3dSculpt";
MString SculptDeformerCmd = "mush3dSculptCmd";
MString CreateLayerCmd = "mush3dCreateLayerCmd";
MString CreateMainWindowCmd = "mush3dCreateMainWindow";

MStatus initializePlugin(MObject plugin)
{
    //MGlobal::startErrorLogging("C:/tmp/mayalog.txt");

    MStatus		status;
    MFnPlugin	pluginFn(plugin, "m3d", "1.0", "Any", &status);

    status = pluginFn.registerCommand(CreateMainWindowCmd,
                                      mush3d::CreateMainWindow::creator,
                                      mush3d::CreateMainWindow::newSyntax);

    CHECK_MSTATUS_AND_RETURN_IT(status);



    status = pluginFn.registerNode(SculptDeformerName, mush3d::SculptDeformer::id,
                                 mush3d::SculptDeformer::creator, mush3d::SculptDeformer::initialize,
                                 MPxNode::kDeformerNode);

    CHECK_MSTATUS_AND_RETURN_IT(status);

    status = pluginFn.registerCommand(SculptDeformerCmd,
                                      mush3d::CreateSculptDeformer::creator);

    CHECK_MSTATUS_AND_RETURN_IT(status);


    status = pluginFn.registerCommand(CreateLayerCmd,
                                      mush3d::CreateLayer::creator,
                                      mush3d::CreateLayer::newSyntax);

    CHECK_MSTATUS_AND_RETURN_IT(status);

    return status;
}


MStatus uninitializePlugin(MObject plugin)
{
    MStatus		status;
    MFnPlugin	pluginFn(plugin, "m3d", "1.0", "Any", &status);

    CHECK_MSTATUS_AND_RETURN_IT(status);

    mush3d::Global::getInstance()->cleanup();

    status = pluginFn.deregisterCommand(CreateMainWindowCmd);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    // deregister deformer
    status = pluginFn.deregisterNode(mush3d::SculptDeformer::id);

    status = pluginFn.deregisterCommand(SculptDeformerCmd);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    status = pluginFn.deregisterCommand(CreateLayerCmd);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    //MGlobal::stopErrorLogging();
    //MGlobal::closeErrorLog();

    return status;
}



