#include "CreateSculptDeformer.h"
#include "SculptDeformer.h"

namespace mush3d{

CreateSculptDeformer::CreateSculptDeformer()
{
}


CreateSculptDeformer::~CreateSculptDeformer() {
}


void* CreateSculptDeformer::creator() {
    return new CreateSculptDeformer;
}


bool CreateSculptDeformer::isUndoable() const {
    return true;
}

MStatus CreateSculptDeformer::doIt(const MArgList& args) {
    MStatus status;

    // get selection
    MSelectionList sel;
    MGlobal::getActiveSelectionList(sel);

    // get selected meshes
    meshes_.clear();
    MItSelectionList itSel(sel, MFn::kTransform);
    int ii = 0;
    MDagPath pathTransform;
    MObject component;
    for (; !itSel.isDone(); itSel.next(), ii++) {
        status = itSel.getDagPath(pathTransform, component);
        CHECK_MSTATUS_AND_RETURN_IT(status);
        status = getShapeNode(pathTransform);
        if (status) {
            meshes_.append(pathTransform);
        }
    }

    if(meshes_.length() == 0)
    {
        MGlobal::displayWarning("no mesh selected");
        return MStatus::kFailure;
    }

    // Build the deformer command string
    std::stringstream ss;
    ss << "deformer -type mush3dSculpt ";
    for (unsigned int i=0; i<meshes_.length(); i++) {
        ss << " " << meshes_[i].partialPathName().asChar();
    }

    MGlobal::displayInfo(ss.str().c_str());

    status = dgMod_.commandToExecute(ss.str().c_str());
    CHECK_MSTATUS_AND_RETURN_IT(status);

    return redoIt();
}


MStatus CreateSculptDeformer::redoIt() {
    MStatus status;

    if(meshes_.length() == 0)
    {
        MGlobal::displayInfo("redo it error");
        return MStatus::kFailure;
    }

    status = dgMod_.doIt();
    CHECK_MSTATUS_AND_RETURN_IT(status);

    // Get the deformer MObject
    MObject oDeformer;
    status = getDeformer(meshes_[0], oDeformer);
    CHECK_MSTATUS_AND_RETURN_IT(status);
    MFnDependencyNode fnNode(oDeformer, &status);
    CHECK_MSTATUS_AND_RETURN_IT(status);
    setResult(fnNode.name());

    // set merged topology

    // get all output meshes combined
    MPlug outputGeomPlug = fnNode.findPlug(SculptDeformer::outputGeom, false);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    unsigned int numMeshes = outputGeomPlug.numElements();

    MIntArray faceCounts;
    MIntArray faceVerts;
    int numVertices=0;

    for(unsigned int i=0; i<numMeshes; i++)
    {
        outputGeomPlug.selectAncestorLogicalIndex(i, SculptDeformer::outputGeom);
        MObject oMesh = outputGeomPlug.asMObject(&status);

        CHECK_MSTATUS_AND_RETURN_IT(status);
        MFnMesh fnMesh(oMesh);

        auto numPolygons = fnMesh.numPolygons();

        for(unsigned int j=0; j<numPolygons; j++)
        {
            MIntArray fVerts;
            fnMesh.getPolygonVertices(j, fVerts);
            auto numFVerts = fVerts.length();

            for(unsigned int k=0; k<numFVerts; k++)
            {
                faceVerts.append(fVerts[k] + numVertices);
            }
            faceCounts.append(numFVerts);
        }

        numVertices += fnMesh.numVertices();
    }

    // set plugs

    // face counts
    {
        MPlug plug = fnNode.findPlug(SculptDeformer::aFaceCounts, false);
        CHECK_MSTATUS_AND_RETURN_IT(status);

        MFnIntArrayData fnData;
        MObject data = fnData.create(faceCounts, &status);
        CHECK_MSTATUS_AND_RETURN_IT(status);

        status = plug.setMObject(data);
        CHECK_MSTATUS_AND_RETURN_IT(status);
    }

    // face verts
    {
        MPlug plug = fnNode.findPlug(SculptDeformer::aFaceVertices, false);
        CHECK_MSTATUS_AND_RETURN_IT(status);

        MFnIntArrayData fnData;
        MObject data = fnData.create(faceVerts, &status);
        CHECK_MSTATUS_AND_RETURN_IT(status);

        status = plug.setMObject(data);
        CHECK_MSTATUS_AND_RETURN_IT(status);
    }

    // num verts
    {
        MPlug plug = fnNode.findPlug(SculptDeformer::aNumVertices, false);
        CHECK_MSTATUS_AND_RETURN_IT(status);

        plug.setInt(numVertices);
    }

    return MS::kSuccess;
}

MStatus CreateSculptDeformer::getShapeNode(MDagPath& path) {
    MStatus status;

    if (path.apiType() == MFn::kMesh) {
        return MS::kSuccess;
    }

    unsigned int shape_count;
    status = path.numberOfShapesDirectlyBelow(shape_count);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    for (unsigned int i = 0; i < shape_count; ++i) {
        status = path.extendToShapeDirectlyBelow(i);
        CHECK_MSTATUS_AND_RETURN_IT(status);

        if (!path.hasFn(MFn::kMesh)) {
            path.pop();
            continue;
        }

        MFnDagNode fnNode(path, &status);
        CHECK_MSTATUS_AND_RETURN_IT(status);
        if (!fnNode.isIntermediateObject()) {
            return MS::kSuccess;
        }
        path.pop();
    }

    return MS::kFailure;
}

// Get the first cvJiggle deformer in the history of pathGeo
MStatus CreateSculptDeformer::getDeformer(MDagPath& pathGeo, MObject& oDeformer) {
    MStatus status;
    status = getShapeNode(pathGeo);
    CHECK_MSTATUS_AND_RETURN_IT(status);
    MObject oMesh = pathGeo.node();
    MItDependencyGraph itDG(oMesh, MFn::kInvalid,
                            MItDependencyGraph::kUpstream,
                            MItDependencyGraph::kDepthFirst,
                            MItDependencyGraph::kNodeLevel, &status);
    CHECK_MSTATUS_AND_RETURN_IT(status);
    for (; !itDG.isDone(); itDG.next()) {
        MFnDependencyNode fnCurrentNode(itDG.currentItem(), &status);
        CHECK_MSTATUS_AND_RETURN_IT(status);
        if (fnCurrentNode.typeId() == SculptDeformer::id) {
            oDeformer = itDG.currentItem();
            return MS::kSuccess;
        }
    }
    std::stringstream ss;
    ss << "No deformer was found in the history of " << pathGeo.partialPathName().asChar();
    MGlobal::displayError(ss.str().c_str());
    return MS::kFailure;
}


MStatus CreateSculptDeformer::undoIt() {
    MStatus status;
    return dgMod_.undoIt();
}

}
