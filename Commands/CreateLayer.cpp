#include "CreateLayer.h"
#include "SculptDeformer.h"

namespace mush3d {

const char *CreateLayer::kDeformerNameFlag = "-d";
const char *CreateLayer::kDeformerNameFlagLong = "-deformer";

CreateLayer::CreateLayer() {}

CreateLayer::~CreateLayer() {}

MSyntax CreateLayer::newSyntax() {
    MSyntax syntax;
    syntax.addFlag(kDeformerNameFlag, kDeformerNameFlagLong, MSyntax::kString);
    syntax.enableQuery(false);
    syntax.enableEdit(false);
    return syntax;
}

void *CreateLayer::creator() { return new CreateLayer; }

bool CreateLayer::isUndoable() const { return false; }

MStatus CreateLayer::doIt(const MArgList &args) {
    MStatus status;

    // get flags
    MArgDatabase argData(syntax(), args);

    // Get flag arguments
    MString deformerName;
    if (argData.isFlagSet(kDeformerNameFlag)) {
        deformerName = argData.flagArgumentString(kDeformerNameFlag, 0, &status);
        CHECK_MSTATUS_AND_RETURN_IT(status);
    }

    // get locator
    MSelectionList selList;
    selList.add(deformerName, status);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    MObject oDeformer;
    status = selList.getDependNode(0, oDeformer);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    // get node
    MFnDependencyNode fnNode(oDeformer);

    // get next avaliable slot and deselect the existing layer
    MPlug layerPlug = fnNode.findPlug(SculptDeformer::aLayer, false);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    unsigned int numElements = layerPlug.numElements();

    unsigned int nextAvaliable = 0;
    for (unsigned int i = 0; i < numElements; i++) {
        MPlug elementPlug = layerPlug[i];
        unsigned int logicalIndex = elementPlug.logicalIndex();

        if (logicalIndex >= nextAvaliable) {
            nextAvaliable = logicalIndex + 1;
        }

        // deselect the layer
        MPlug selectionPlug = elementPlug.child(SculptDeformer::aSelected);
        CHECK_MSTATUS_AND_RETURN_IT(status);

        selectionPlug.setBool(false);
    }

    // set the new layer selected, it will also create the layer
    layerPlug.selectAncestorLogicalIndex(nextAvaliable, SculptDeformer::aLayer);
    MPlug selectionPlug = layerPlug.child(SculptDeformer::aSelected);
    selectionPlug.setBool(true);

    // get all points merged
    MPlug outputGeomPlug = fnNode.findPlug(SculptDeformer::outputGeom, false);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    unsigned int numMeshes = outputGeomPlug.numElements();

    MPointArray allPoints;

    for(unsigned int i=0; i<numMeshes; i++)
    {
        outputGeomPlug.selectAncestorLogicalIndex(i, SculptDeformer::outputGeom);
        MObject oMesh = outputGeomPlug.asMObject(&status);

        MFnMesh fnMesh(oMesh);
        MPointArray pts;
        fnMesh.getPoints(pts);

        for(unsigned int j=0; j<pts.length(); j++)
        {
            allPoints.append(pts[j]);
        }
    }

    // set plugs

    // base points
    {
        MPlug plug = layerPlug.child(SculptDeformer::aBasePoints);
        CHECK_MSTATUS_AND_RETURN_IT(status);

        MFnPointArrayData fnData;
        MObject data = fnData.create(allPoints, &status);
        CHECK_MSTATUS_AND_RETURN_IT(status);

        status = plug.setMObject(data);
        CHECK_MSTATUS_AND_RETURN_IT(status);
    }

    // offset points
    {
        MPlug plug = layerPlug.child(SculptDeformer::aOffsetPoints);
        CHECK_MSTATUS_AND_RETURN_IT(status);

        MFnPointArrayData fnData;
        MObject data = fnData.create(allPoints, &status);
        CHECK_MSTATUS_AND_RETURN_IT(status);

        status = plug.setMObject(data);
        CHECK_MSTATUS_AND_RETURN_IT(status);
    }

    setResult(nextAvaliable);

    return status;
}

} // namespace mush3d
